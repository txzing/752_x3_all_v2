#include "../bsp.h"
#include "inject_mode.h"
#include <string.h>

#if defined (XPAR_XV_TPG_NUM_INSTANCES) && (XPAR_XV_TPG_NUM_INSTANCES > 0)

/* VTC / XVidC timing fields are u16 */
#define VIDEO_TOTAL_MAX    65535U
/* Reject if achieved fps would be more than this below request */
#define VIDEO_FPS_SLACK    1U

/*
 * EEPROM geometry NVM - see eeprom.h (EEPROM_ADDR_VIDEO_GEOM).
 * v2: 48 bytes as three 16-byte pages @0x80/0x90/0xA0 (single-page writes).
 *   [0]     checksum(~sum of [1..47])
 *   [1..4]  magic 'VEM2'
 *   [5]     version=2
 *   [6]     ch_count
 *   [7]     rsvd
 *   per ch (19B) x2 starting @8:
 *     wLE hLE fps | h_fp h_sw h_bp h_pol | v_fp v_sw v_bp v_pol
 *   buf[7] and bytes 46..47 are reserved (checksummed zeros).
 */
#define VIDEO_GEOM_NV_MAGIC0 0x56U
#define VIDEO_GEOM_NV_MAGIC1 0x45U
#define VIDEO_GEOM_NV_MAGIC2 0x4DU
#define VIDEO_GEOM_NV_MAGIC3 0x32U /* 'VEM2' */
#define VIDEO_GEOM_NV_VER    2U
#define VIDEO_GEOM_NV_CH_MAX 2U
#define VIDEO_GEOM_NV_ADDR   EEPROM_ADDR_VIDEO_GEOM
#define VIDEO_GEOM_NV_BYTES  EEPROM_VIDEO_GEOM_BYTES
#define VIDEO_GEOM_NV_PAGE   16U
#define VIDEO_GEOM_CH_BYTES  19U

static video_src_cfg_t s_src[VIDEO_SRC_COUNT];
static u8 s_have_geom;
static u16 s_prev_w[VIDEO_SRC_COUNT];
static u16 s_prev_h[VIDEO_SRC_COUNT];
/*
 * Last CSI WC width programmed per CSI GPIO path (skip redundant writes).
 * [0] = PathA ST_0 -> CSI0/1/2; [1] = CSI3 ST_1 (PathA or PathB width).
 * Per-channel valid: do not share one flag (route must not poison PathA).
 */
static u16 s_csi_w[VIDEO_SRC_COUNT];
static u8 s_csi_w_valid[VIDEO_SRC_COUNT];
/* 1 = runtime geom differs from last successful EEPROM image. */
static u8 s_nv_dirty;

static void inject_csi_shadow_invalidate(void)
{
	u8 i;

	for (i = 0U; i < (u8)VIDEO_SRC_COUNT; i++)
	{
		s_csi_w_valid[i] = 0U;
		s_csi_w[i] = 0U;
	}
}

static int video_timing_resolve(u16 width, u16 height, u8 fps,
				video_src_cfg_t *out);

static u8 video_geom_nv_csum(const u8 *p, u16 n)
{
	u16 i;
	u8 calc = 0U;

	for (i = 0U; i < n; i++)
	{
		calc = (u8)(calc + p[i]);
	}
	return (u8)(~calc & 0xFFU);
}

static void video_geom_nv_put_u16(u8 *p, u16 v)
{
	p[0] = (u8)(v & 0xFFU);
	p[1] = (u8)((v >> 8) & 0xFFU);
}

static u16 video_geom_nv_get_u16(const u8 *p)
{
	return (u16)p[0] | ((u16)p[1] << 8);
}

static void video_geom_nv_put_ch(u8 *p, const video_src_cfg_t *cfg)
{
	video_geom_nv_put_u16(&p[0], cfg->width);
	video_geom_nv_put_u16(&p[2], cfg->height);
	p[4] = cfg->fps;
	video_geom_nv_put_u16(&p[5], (u16)cfg->h_fp);
	video_geom_nv_put_u16(&p[7], (u16)cfg->h_sw);
	video_geom_nv_put_u16(&p[9], (u16)cfg->h_bp);
	p[11] = (u8)(cfg->h_pol & 0xFFU);
	video_geom_nv_put_u16(&p[12], (u16)cfg->v_fp);
	video_geom_nv_put_u16(&p[14], (u16)cfg->v_sw);
	video_geom_nv_put_u16(&p[16], (u16)cfg->v_bp);
	p[18] = (u8)(cfg->v_pol & 0xFFU);
}

static void video_geom_nv_get_ch(const u8 *p, video_src_cfg_t *cfg)
{
	cfg->width = video_geom_nv_get_u16(&p[0]);
	cfg->height = video_geom_nv_get_u16(&p[2]);
	cfg->fps = p[4];
	cfg->h_fp = video_geom_nv_get_u16(&p[5]);
	cfg->h_sw = video_geom_nv_get_u16(&p[7]);
	cfg->h_bp = video_geom_nv_get_u16(&p[9]);
	cfg->h_pol = p[11];
	cfg->v_fp = video_geom_nv_get_u16(&p[12]);
	cfg->v_sw = video_geom_nv_get_u16(&p[14]);
	cfg->v_bp = video_geom_nv_get_u16(&p[16]);
	cfg->v_pol = p[18];
	cfg->is_custom = 1U;
	cfg->clk_khz = 0U;
}

static int video_geom_nv_write_pages(u8 *buf)
{
	u16 off;
	s32 Status;

	/* One aligned page per call at absolute 0x80/0x90/0xA0. */
	for (off = 0U; off < VIDEO_GEOM_NV_BYTES; off = (u16)(off + VIDEO_GEOM_NV_PAGE))
	{
		Status = EepromWriteVerify(&ps_i2c_0, &buf[off], VIDEO_GEOM_NV_PAGE,
					   (u16)(VIDEO_GEOM_NV_ADDR + off));
		if (Status != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
	}
	return XST_SUCCESS;
}

static int video_geom_nv_read_pages(u8 *buf)
{
	u16 off;
	s32 Status;

	for (off = 0U; off < VIDEO_GEOM_NV_BYTES; off = (u16)(off + VIDEO_GEOM_NV_PAGE))
	{
		Status = EepromReadData(&ps_i2c_0, &buf[off], VIDEO_GEOM_NV_PAGE,
					(u16)(VIDEO_GEOM_NV_ADDR + off));
		if (Status != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
	}
	return XST_SUCCESS;
}

static int video_geom_nv_save(void)
{
#if defined (USING_EEPROM) && defined (__EEPROM_H__)
	u8 buf[VIDEO_GEOM_NV_BYTES];
	u8 ch;
	u8 nch;

	memset(buf, 0, sizeof(buf));
	buf[1] = VIDEO_GEOM_NV_MAGIC0;
	buf[2] = VIDEO_GEOM_NV_MAGIC1;
	buf[3] = VIDEO_GEOM_NV_MAGIC2;
	buf[4] = VIDEO_GEOM_NV_MAGIC3;
	buf[5] = VIDEO_GEOM_NV_VER;

	nch = (u8)VIDEO_SRC_COUNT;
	if (nch > VIDEO_GEOM_NV_CH_MAX)
	{
		nch = VIDEO_GEOM_NV_CH_MAX;
	}
	buf[6] = nch;

	for (ch = 0U; ch < VIDEO_GEOM_NV_CH_MAX; ch++)
	{
		u8 *slot = &buf[8U + (u8)(ch * VIDEO_GEOM_CH_BYTES)];

		if (ch < nch)
		{
			video_geom_nv_put_ch(slot, &s_src[ch]);
		}
		else
		{
			video_src_cfg_t tmp;

			memset(&tmp, 0, sizeof(tmp));
			tmp.width = VIDEO_DEFAULT_W;
			tmp.height = VIDEO_DEFAULT_H;
			tmp.fps = VIDEO_DEFAULT_FPS;
			tmp.h_fp = 88U;
			tmp.h_sw = 44U;
			tmp.h_bp = 148U;
			tmp.h_pol = 1U;
			tmp.v_fp = 4U;
			tmp.v_sw = 5U;
			tmp.v_bp = 36U;
			tmp.v_pol = 1U;
			video_geom_nv_put_ch(slot, &tmp);
		}
	}
	buf[0] = video_geom_nv_csum(&buf[1], (u16)(VIDEO_GEOM_NV_BYTES - 1U));

	if (video_geom_nv_write_pages(buf) != XST_SUCCESS)
	{
		xil_printf("video_geom_nv: save failed\r\n");
		return XST_FAILURE;
	}
	xil_printf("video_geom_nv: saved");
	for (ch = 0U; ch < nch; ch++)
	{
		xil_printf(" ch%u=%ux%u@%u", (unsigned)ch,
			   (unsigned)s_src[ch].width,
			   (unsigned)s_src[ch].height,
			   (unsigned)s_src[ch].fps);
	}
	xil_printf("\r\n");
	return XST_SUCCESS;
#else
	return XST_FAILURE; /* no EEPROM build */
#endif
}

/*
 * Explicit 0x51 sub=4: write only dirty regions.
 * - Geom fail: abort, no TPG write, dirty flags unchanged.
 * - Geom OK then TPG fail: leave geom dirty set so retry rewrites both.
 * - Both clean: skip I2C.
 */
int video_inject_save_nv(void)
{
	u8 need_geom;
	u8 need_pat;
	int Status;

	if (s_have_geom == 0U)
	{
		xil_printf("video_geom_nv: nothing to save\r\n");
		return XST_FAILURE;
	}

	need_geom = s_nv_dirty;
#if defined (XPAR_XV_TPG_NUM_INSTANCES)
	need_pat = tpg_nv_is_dirty();
#else
	need_pat = 0U;
#endif

	if ((need_geom == 0U) && (need_pat == 0U))
	{
		xil_printf("video_nv: clean, skip write\r\n");
		return XST_SUCCESS;
	}

	if (need_geom != 0U)
	{
		Status = video_geom_nv_save();
		if (Status != XST_SUCCESS)
		{
			return Status;
		}
		/* Clear dirty only after TPG also succeeds (or TPG not needed). */
	}

#if defined (XPAR_XV_TPG_NUM_INSTANCES)
	if (need_pat != 0U)
	{
		Status = tpg_nv_save();
		if (Status != XST_SUCCESS)
		{
			/* Geom may already be in EEPROM; keep s_nv_dirty for retry. */
			xil_printf("video_nv: tpg save fail, geom dirty kept\r\n");
			return Status;
		}
	}
#endif

	if (need_geom != 0U)
	{
		s_nv_dirty = 0U;
	}
	return XST_SUCCESS;
}


/* Load full configs into out[]; returns XST_SUCCESS only if NVM valid. */
static int video_geom_nv_load(video_src_cfg_t *out, u8 nch)
{
#if defined (USING_EEPROM) && defined (__EEPROM_H__)
	u8 buf[VIDEO_GEOM_NV_BYTES];
	u8 ch;
	u8 expect;

	if ((out == NULL) || (nch == 0U))
	{
		return XST_FAILURE;
	}

	memset(buf, 0, sizeof(buf));
	if (video_geom_nv_read_pages(buf) != XST_SUCCESS)
	{
		xil_printf("video_geom_nv: read failed\r\n");
		return XST_FAILURE;
	}

	if (buf[0] != video_geom_nv_csum(&buf[1],
					 (u16)(VIDEO_GEOM_NV_BYTES - 1U)))
	{
		xil_printf("video_geom_nv: checksum fail\r\n");
		return XST_FAILURE;
	}
	if ((buf[1] != VIDEO_GEOM_NV_MAGIC0) ||
	    (buf[2] != VIDEO_GEOM_NV_MAGIC1) ||
	    (buf[3] != VIDEO_GEOM_NV_MAGIC2) ||
	    (buf[4] != VIDEO_GEOM_NV_MAGIC3) ||
	    (buf[5] != VIDEO_GEOM_NV_VER))
	{
		xil_printf("video_geom_nv: bad magic/ver\r\n");
		return XST_FAILURE;
	}

	expect = nch;
	if (expect > VIDEO_GEOM_NV_CH_MAX)
	{
		expect = VIDEO_GEOM_NV_CH_MAX;
	}
	if (buf[6] < expect)
	{
		xil_printf("video_geom_nv: ch_count %u < %u\r\n",
			   (unsigned)buf[6], (unsigned)expect);
		return XST_FAILURE;
	}

	for (ch = 0U; ch < expect; ch++)
	{
		video_geom_nv_get_ch(&buf[8U + (u8)(ch * VIDEO_GEOM_CH_BYTES)],
				    &out[ch]);
		if ((out[ch].fps == 0U) ||
		    (out[ch].width <= VIDEO_MIN_ACTIVE) ||
		    (out[ch].height <= VIDEO_MIN_ACTIVE) ||
		    ((out[ch].width & 1U) != 0U))
		{
			xil_printf("video_geom_nv: invalid ch%u %ux%u@%u\r\n",
				   (unsigned)ch,
				   (unsigned)out[ch].width,
				   (unsigned)out[ch].height,
				   (unsigned)out[ch].fps);
			return XST_FAILURE;
		}
	}
	xil_printf("video_geom_nv: load ch0=%ux%u@%u",
		   (unsigned)out[0].width, (unsigned)out[0].height,
		   (unsigned)out[0].fps);
	if (expect > 1U)
	{
		xil_printf(" ch1=%ux%u@%u",
			   (unsigned)out[1].width, (unsigned)out[1].height,
			   (unsigned)out[1].fps);
	}
	xil_printf("\r\n");
	return XST_SUCCESS;
#else
	(void)out;
	(void)nch;
	return XST_FAILURE;
#endif
}

static int video_inject_fill_factory(void)
{
	u8 ch;
	int Status;

	for (ch = 0U; ch < (u8)VIDEO_SRC_COUNT; ch++)
	{
		Status = video_timing_resolve(VIDEO_DEFAULT_W, VIDEO_DEFAULT_H,
					      VIDEO_DEFAULT_FPS, &s_src[ch]);
		if (Status != XST_SUCCESS)
		{
			xil_printf("video_inject: default resolve fail\r\n");
			return Status;
		}
	}
	return XST_SUCCESS;
}

const video_src_cfg_t *video_src_cfg_get(u8 ch)
{
	if (ch >= (u8)VIDEO_SRC_COUNT)
	{
		return NULL;
	}
	return &s_src[ch];
}

u8 video_src_host_count(void)
{
	return (u8)VIDEO_SRC_COUNT;
}

u8 video_src_source_type(u8 idx)
{
	if (idx >= (u8)VIDEO_SRC_COUNT)
	{
		return VIDEO_CH_SRC_NONE;
	}
	return VIDEO_CH_SRC_TPG;
}

static void video_cfg_fill(video_src_cfg_t *out, u16 w, u16 h, u8 fps,
			   u8 is_custom, u32 clk_khz,
			   u32 h_fp, u32 h_sw, u32 h_bp, u32 h_pol,
			   u32 v_fp, u32 v_sw, u32 v_bp, u32 v_pol)
{
	out->width = w;
	out->height = h;
	out->fps = fps;
	out->is_custom = is_custom;
	out->clk_khz = clk_khz;
	out->h_fp = h_fp;
	out->h_sw = h_sw;
	out->h_bp = h_bp;
	out->h_pol = h_pol;
	out->v_fp = v_fp;
	out->v_sw = v_sw;
	out->v_bp = v_bp;
	out->v_pol = v_pol;
}

static u8 video_cfg_same_geom(const video_src_cfg_t *a, const video_src_cfg_t *b)
{
	return (u8)((a->width == b->width) && (a->height == b->height) &&
		    (a->fps == b->fps) && (a->clk_khz == b->clk_khz) &&
		    (a->h_fp == b->h_fp) && (a->h_sw == b->h_sw) &&
		    (a->h_bp == b->h_bp) && (a->h_pol == b->h_pol) &&
		    (a->v_fp == b->v_fp) && (a->v_sw == b->v_sw) &&
		    (a->v_bp == b->v_bp) && (a->v_pol == b->v_pol));
}

/*
 * Compute pixel clock from blanking, snap to clk_wiz.
 * lock_blanking=1: keep host HFP/HSW/HBP (no pad); reject on fps undershoot.
 * lock_blanking=0: may pad HBP so snapped pclk still meets fps (auto path).
 */
static int video_cfg_from_blanking(video_src_cfg_t *out, u16 width, u16 height,
				   u8 fps, u8 is_custom, u8 lock_blanking,
				   u32 h_fp, u32 h_sw, u32 h_bp, u32 h_pol,
				   u32 v_fp, u32 v_sw, u32 v_bp, u32 v_pol)
{
	u32 htotal = (u32)width + h_fp + h_sw + h_bp;
	u32 vtotal = (u32)height + v_fp + v_sw + v_bp;
	u32 want_khz;
	u32 snap_khz;
	u64 target_htotal;
	u64 act_fps;

	if ((htotal > VIDEO_TOTAL_MAX) || (vtotal > VIDEO_TOTAL_MAX) ||
	    (htotal == 0U) || (vtotal == 0U))
	{
		video_cfg_fill(out, width, height, fps, is_custom, 0U,
			       h_fp, h_sw, h_bp, h_pol,
			       v_fp, v_sw, v_bp, v_pol);
		return XST_FAILURE;
	}

	want_khz = (u32)((((u64)htotal * (u64)vtotal * (u64)fps) + 500ULL) /
			 1000ULL);
	if ((want_khz == 0U) || (want_khz > VIDEO_PCLK_MAX_KHZ))
	{
		video_cfg_fill(out, width, height, fps, is_custom, want_khz,
			       h_fp, h_sw, h_bp, h_pol,
			       v_fp, v_sw, v_bp, v_pol);
		return XST_FAILURE;
	}

	snap_khz = clkwiz_pick_pclk_khz(want_khz, VIDEO_PCLK_MAX_KHZ);
	if ((snap_khz == 0U) || (snap_khz > VIDEO_PCLK_MAX_KHZ))
	{
		video_cfg_fill(out, width, height, fps, is_custom, snap_khz,
			       h_fp, h_sw, h_bp, h_pol,
			       v_fp, v_sw, v_bp, v_pol);
		return XST_FAILURE;
	}

	if (lock_blanking == 0U)
	{
		/* Stretch H blank so fps  ~=  snap / (Htotal * Vtotal). */
		target_htotal = ((u64)snap_khz * 1000ULL) /
				((u64)vtotal * (u64)fps);
		if (target_htotal > (u64)htotal)
		{
			u32 pad = (u32)(target_htotal - (u64)htotal);

			if ((h_bp > (VIDEO_TOTAL_MAX - pad)) ||
			    ((htotal + pad) > VIDEO_TOTAL_MAX))
			{
				video_cfg_fill(out, width, height, fps, is_custom,
					       snap_khz,
					       h_fp, h_sw, h_bp, h_pol,
					       v_fp, v_sw, v_bp, v_pol);
				return XST_FAILURE;
			}
			h_bp += pad;
			htotal += pad;
		}
	}

	act_fps = ((u64)snap_khz * 1000ULL) / ((u64)htotal * (u64)vtotal);
	/* Reject both undershoot and overshoot beyond +/-VIDEO_FPS_SLACK. */
	if (act_fps + (u64)VIDEO_FPS_SLACK < (u64)fps)
	{
		xil_printf("video_timing: fps undershoot %u->%u reject\r\n",
			   (unsigned)fps, (unsigned)act_fps);
		video_cfg_fill(out, width, height, fps, is_custom, snap_khz,
			       h_fp, h_sw, h_bp, h_pol,
			       v_fp, v_sw, v_bp, v_pol);
		return XST_FAILURE;
	}
	if (act_fps > ((u64)fps + (u64)VIDEO_FPS_SLACK))
	{
		xil_printf("video_timing: fps overshoot %u->%u reject\r\n",
			   (unsigned)fps, (unsigned)act_fps);
		video_cfg_fill(out, width, height, fps, is_custom, snap_khz,
			       h_fp, h_sw, h_bp, h_pol,
			       v_fp, v_sw, v_bp, v_pol);
		return XST_FAILURE;
	}

	video_cfg_fill(out, width, height, fps, is_custom, snap_khz,
		       h_fp, h_sw, h_bp, h_pol, v_fp, v_sw, v_bp, v_pol);
	if (snap_khz != want_khz)
	{
		xil_printf("video_timing: clk %u -> %u kHz Hbp=%u%s\r\n",
			   (unsigned)want_khz, (unsigned)snap_khz,
			   (unsigned)out->h_bp,
			   (lock_blanking != 0U) ? " (locked)" : "");
	}
	return XST_SUCCESS;
}

static void video_blanking_default(u16 width, u16 height,
				   u32 *h_fp, u32 *h_sw, u32 *h_bp, u32 *h_pol,
				   u32 *v_fp, u32 *v_sw, u32 *v_bp, u32 *v_pol)
{
	if (height > 1080U)
	{
		*h_fp = 48U;
		*h_sw = 32U;
		*h_bp = 80U;
		*v_fp = 8U;
		*v_sw = 5U;
		*v_bp = 20U;
	}
	else if (width < 640U)
	{
		*h_fp = 16U;
		*h_sw = 32U;
		*h_bp = 48U;
		*v_fp = 4U;
		*v_sw = 5U;
		*v_bp = 36U;
	}
	else
	{
		*h_fp = 88U;
		*h_sw = 44U;
		*h_bp = 148U;
		*v_fp = 4U;
		*v_sw = 5U;
		*v_bp = 36U;
	}
	*h_pol = 1U;
	*v_pol = 1U;
}

static int video_timing_resolve(u16 width, u16 height, u8 fps,
				video_src_cfg_t *out)
{
#if defined (XPAR_XVTC_NUM_INSTANCES)
	u32 i;
	u32 h_fp, h_sw, h_bp, h_pol;
	u32 v_fp, v_sw, v_bp, v_pol;
	int Status;
#endif

	if (out == NULL)
	{
		return XST_FAILURE;
	}
	if ((fps == 0U) ||
	    (width <= VIDEO_MIN_ACTIVE) || (height <= VIDEO_MIN_ACTIVE) ||
	    ((width & 1U) != 0U))
	{
		xil_printf("video_timing: reject %ux%u@%u (even W>%u, H>%u)\r\n",
			   (unsigned)width, (unsigned)height, (unsigned)fps,
			   (unsigned)VIDEO_MIN_ACTIVE, (unsigned)VIDEO_MIN_ACTIVE);
		return XST_FAILURE;
	}

#if defined (XPAR_XVTC_NUM_INSTANCES)
	for (i = 0U; i < (u32)VIDEO_RESOLUTION_NO_BUTT; i++)
	{
		const vres_timing_t *t = &vres_resolutions[i];

		if ((t->HActiveVideo != (u32)width) ||
		    (t->VActiveVideo != (u32)height))
		{
			continue;
		}
		Status = video_cfg_from_blanking(out, width, height, fps, 0U, 0U,
						 t->HFrontPorch, t->HSyncWidth,
						 t->HBackPorch, t->HSyncPolarity,
						 t->VFrontPorch, t->VSyncWidth,
						 t->VBackPorch, t->VSyncPolarity);
		if (Status != XST_SUCCESS)
		{
			xil_printf("video_timing: reject %ux%u@%u pclk=%u\r\n",
				   (unsigned)width, (unsigned)height,
				   (unsigned)fps, (unsigned)out->clk_khz);
		}
		return Status;
	}

	video_blanking_default(width, height,
			       &h_fp, &h_sw, &h_bp, &h_pol,
			       &v_fp, &v_sw, &v_bp, &v_pol);
	Status = video_cfg_from_blanking(out, width, height, fps, 1U, 0U,
					 h_fp, h_sw, h_bp, h_pol,
					 v_fp, v_sw, v_bp, v_pol);
	if (Status != XST_SUCCESS)
	{
		xil_printf("video_timing: reject %ux%u@%u pclk=%u\r\n",
			   (unsigned)width, (unsigned)height, (unsigned)fps,
			   (unsigned)out->clk_khz);
		return Status;
	}
	xil_printf("video_timing: custom %ux%u@%u clk=%u\r\n",
		   (unsigned)width, (unsigned)height, (unsigned)fps,
		   (unsigned)out->clk_khz);
	return XST_SUCCESS;
#else
	(void)width;
	(void)height;
	(void)fps;
	return XST_FAILURE;
#endif
}

static void video_cfg_to_vres(const video_src_cfg_t *cfg, vres_timing_t *t)
{
	t->pName = (cfg->is_custom != 0U) ? "CUSTOM" : "TABLE";
	t->HActiveVideo = cfg->width;
	t->HFrontPorch = cfg->h_fp;
	t->HSyncWidth = cfg->h_sw;
	t->HBackPorch = cfg->h_bp;
	t->HSyncPolarity = cfg->h_pol;
	t->VActiveVideo = cfg->height;
	t->VFrontPorch = cfg->v_fp;
	t->VSyncWidth = cfg->v_sw;
	t->VBackPorch = cfg->v_bp;
	t->VSyncPolarity = cfg->v_pol;
}

static int inject_apply_src(u8 ch, const video_src_cfg_t *cfg, u8 light)
{
	int Status;

	Status = tpg_apply_geometry(ch, cfg->height, cfg->width);
	if (Status != XST_SUCCESS)
	{
		return Status;
	}

	if (light != 0U)
	{
		return XST_SUCCESS;
	}

#if defined (XPAR_XVPROCSS_NUM_INSTANCES)
	if (ch >= (u8)XPAR_XVPROCSS_NUM_INSTANCES)
	{
		return XST_FAILURE;
	}
	Status = vpss_config_csc_timing((u32)ch,
					cfg->width, cfg->height, cfg->fps,
					cfg->h_fp, cfg->h_sw, cfg->h_bp, cfg->h_pol,
					cfg->v_fp, cfg->v_sw, cfg->v_bp, cfg->v_pol);
	if (Status != XST_SUCCESS)
	{
		return Status;
	}
#endif
	return XST_SUCCESS;
}

#if defined (XPAR_TPG_0_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR)
#define VIDEO_READY_FPS_TOL     1U
#define VIDEO_READY_TIMEOUT_MS  2500U
#define VIDEO_READY_POLL_MS     20U
#define VIDEO_READY_STABLE_N    2U

/* TPG-side mon (debug / legacy). */
static u32 video_tpg_mon_base(u8 ch)
{
	if (ch == VIDEO_SRC_0)
	{
		return (u32)XPAR_TPG_0_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR;
	}
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2) && \
    defined (XPAR_TPG_1_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR)
	if (ch == VIDEO_SRC_1)
	{
		return (u32)XPAR_TPG_1_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR;
	}
#endif
	return 0U;
}

/* Post-VTC stream mon (into CSI path). Prefer this for ready-check. */
static u32 video_stream_mon_base(u8 ch)
{
#if defined (XPAR_ST_0_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR)
	if (ch == VIDEO_SRC_0)
	{
		return (u32)XPAR_ST_0_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR;
	}
#endif
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2) && \
    defined (XPAR_ST_1_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR)
	if (ch == VIDEO_SRC_1)
	{
		return (u32)XPAR_ST_1_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR;
	}
#endif
	return video_tpg_mon_base(ch);
}

static int video_mon_read(u32 base, u32 *out_w, u32 *out_h, u32 *out_fps)
{
	u32 w1;
	u32 h1;

	if ((base == 0U) || (out_w == NULL) || (out_h == NULL))
	{
		return 0;
	}
	w1 = Xil_In32(base);
	h1 = Xil_In32(base + 4U);
	*out_w = w1;
	*out_h = h1;
	if ((w1 < 10U) || (h1 < 10U))
	{
		return 0;
	}
	if (out_fps != NULL)
	{
		*out_fps = Xil_In32(base + 8U);
	}
	return 1;
}

/* Poll mon until W/H exact and fps within +/-1. Timeout = failure. */
static int video_wait_ready_ch(u8 ch, u8 use_tpg_mon)
{
	u32 base;
	u32 elapsed;
	u32 w;
	u32 h;
	u32 fps;
	u32 stable;
	u32 fps_lo;
	u32 fps_hi;

	if ((ch >= (u8)VIDEO_SRC_COUNT) || (s_have_geom == 0U))
	{
		return XST_SUCCESS;
	}
	base = (use_tpg_mon != 0U) ? video_tpg_mon_base(ch) :
				     video_stream_mon_base(ch);
	if (base == 0U)
	{
		return XST_SUCCESS;
	}

	fps_lo = (s_src[ch].fps > VIDEO_READY_FPS_TOL) ?
		 (u32)(s_src[ch].fps - VIDEO_READY_FPS_TOL) : 0U;
	fps_hi = (u32)s_src[ch].fps + VIDEO_READY_FPS_TOL;
	stable = 0U;
	w = 0U;
	h = 0U;
	fps = 0U;

	for (elapsed = 0U; elapsed < VIDEO_READY_TIMEOUT_MS;
	     elapsed += VIDEO_READY_POLL_MS)
	{
		if (video_mon_read(base, &w, &h, &fps) != 0)
		{
			if ((w == (u32)s_src[ch].width) &&
			    (h == (u32)s_src[ch].height) &&
			    (fps >= fps_lo) && (fps <= fps_hi))
			{
				stable++;
				if (stable >= VIDEO_READY_STABLE_N)
				{
					return XST_SUCCESS;
				}
			}
			else
			{
				stable = 0U;
			}
		}
		else
		{
			stable = 0U;
		}
		usleep(VIDEO_READY_POLL_MS * 1000U);
	}
	xil_printf("video_wait: ch%u timeout %s=%ux%u@%u want=%ux%u@%u\r\n",
		   (unsigned)ch,
		   (use_tpg_mon != 0U) ? "tpg" : "st",
		   (unsigned)w, (unsigned)h, (unsigned)fps,
		   (unsigned)s_src[ch].width, (unsigned)s_src[ch].height,
		   (unsigned)s_src[ch].fps);
#if defined (XPAR_XVTC_NUM_INSTANCES)
	{
		u32 tpg_w = 0U;
		u32 tpg_h = 0U;
		u32 tpg_fps = 0U;
		u32 ctl0;
		u32 ctl1;
		u32 tpg_base = video_tpg_mon_base(ch);

		(void)video_mon_read(tpg_base, &tpg_w, &tpg_h, &tpg_fps);
		ctl0 = XVtc_ReadReg(VtcInst0.Config.BaseAddress, XVTC_CTL_OFFSET);
		ctl1 = XVtc_ReadReg(VtcInst1.Config.BaseAddress, XVTC_CTL_OFFSET);
		xil_printf("video_wait: ch%u tpg_mon=%ux%u@%u vtc_ctl0=0x%08x ctl1=0x%08x\r\n",
			   (unsigned)ch, (unsigned)tpg_w, (unsigned)tpg_h,
			   (unsigned)tpg_fps, (unsigned)ctl0, (unsigned)ctl1);
	}
#endif
	return XST_FAILURE;
}

static u8 inject_channels_same_wh_fps(void);

/* Dump ST+TPG for both channels (debug wait / stuck). */
static void video_wait_dump_all(const char *tag)
{
	u8 ch;

	xil_printf("video_wait: dump %s same=%u\r\n",
		   (tag != NULL) ? tag : "-",
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
		   (unsigned)inject_channels_same_wh_fps()
#else
		   0U
#endif
		   );
	for (ch = 0U; ch < (u8)VIDEO_SRC_COUNT; ch++)
	{
		u32 st_w = 0U;
		u32 st_h = 0U;
		u32 st_f = 0U;
		u32 tpg_w = 0U;
		u32 tpg_h = 0U;
		u32 tpg_f = 0U;

		(void)video_mon_read(video_stream_mon_base(ch), &st_w, &st_h, &st_f);
		(void)video_mon_read(video_tpg_mon_base(ch), &tpg_w, &tpg_h, &tpg_f);
		xil_printf("video_wait: ch%u want=%ux%u@%u st=%ux%u@%u tpg=%ux%u@%u\r\n",
			   (unsigned)ch,
			   (unsigned)s_src[ch].width, (unsigned)s_src[ch].height,
			   (unsigned)s_src[ch].fps,
			   (unsigned)st_w, (unsigned)st_h, (unsigned)st_f,
			   (unsigned)tpg_w, (unsigned)tpg_h, (unsigned)tpg_f);
	}
}

static int video_wait_ready_all(void)
{
	u8 ch;
	int Status = XST_SUCCESS;
	u8 same = 0U;

#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
	same = inject_channels_same_wh_fps();
#endif
	for (ch = 0U; ch < (u8)VIDEO_SRC_COUNT; ch++)
	{
		/*
		 * TREADY+FIFO: PathA ST should stay live when CSI3<-PathB.
		 * Same W/H/fps: CSI3<-PathA, PathB SI parked ? skip ST1.
		 * Diff: wait ST both; if PathA ST still dead (legacy backpressure),
		 * fall back to TPG once and print  -  avoids false NACK storms.
		 */
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
		if ((same != 0U) && (ch == VIDEO_SRC_1))
		{
			continue;
		}
#endif
		if (video_wait_ready_ch(ch, 0U) != XST_SUCCESS)
		{
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
			if ((same == 0U) && (ch == VIDEO_SRC_0) &&
			    (video_wait_ready_ch(ch, 1U) == XST_SUCCESS))
			{
				xil_printf("video_wait: ch0 ST dead, TPG ok (PathA backpressure?)\r\n");
			}
			else
#endif
			{
				Status = XST_FAILURE;
			}
		}
	}
	if (Status != XST_SUCCESS)
	{
		video_wait_dump_all("after-timeout");
	}
	return Status;
}
#else
static int video_wait_ready_all(void)
{
	return XST_SUCCESS;
}
#endif

static u8 inject_need_pipe_reset_ch(u8 ch)
{
	if (s_have_geom == 0U)
	{
		return 0U;
	}
	if (ch >= (u8)VIDEO_SRC_COUNT)
	{
		return 0U;
	}
	if ((s_src[ch].height != s_prev_h[ch]) ||
	    (s_src[ch].width != s_prev_w[ch]))
	{
		return 1U;
	}
	return 0U;
}

static u8 inject_need_pipe_reset_any(void)
{
	u8 ch;

	for (ch = 0U; ch < (u8)VIDEO_SRC_COUNT; ch++)
	{
		if (inject_need_pipe_reset_ch(ch) != 0U)
		{
			return 1U;
		}
	}
	return 0U;
}

static int inject_apply_vtc_ch(u8 ch);
static int inject_apply_csi_ch(u8 ch);
static int inject_csi3_route_update(void);

#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
/* Same W/H/fps: CSI3 <- PathA (merge); else CSI3 <- PathB. */
static u8 inject_channels_same_wh_fps(void)
{
	return (u8)((s_src[VIDEO_SRC_0].width == s_src[VIDEO_SRC_1].width) &&
		    (s_src[VIDEO_SRC_0].height == s_src[VIDEO_SRC_1].height) &&
		    (s_src[VIDEO_SRC_0].fps == s_src[VIDEO_SRC_1].fps));
}
#endif

static int inject_csi3_route_update(void)
{
#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES) && \
    (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
	u8 same = inject_channels_same_wh_fps();

	/*
	 * Same W/H/fps -> CSI3 <- PathA; else CSI3 <- PathB.
	 * Mux only here; CSI WC + Activate are in inject_apply_csi_ch so a
	 * resolution change always reprograms the matching CSI ports.
	 * Switch glitch during mux is acceptable.
	 */
	return axis_switch_csi3_select(same);
#else
	return XST_SUCCESS;
#endif
}

/* CSI3 WC follows active mux source width. */
static u16 inject_csi3_wc_width(void)
{
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
	if (inject_channels_same_wh_fps() != 0U)
	{
		return s_src[VIDEO_SRC_0].width;
	}
	return s_src[VIDEO_SRC_1].width;
#else
	return s_src[VIDEO_SRC_0].width;
#endif
}

static int inject_clkwiz_apply_current(u8 *did_reconfig)
{
	u32 clk0_khz;
	u32 clk1_khz;

	clk0_khz = s_src[VIDEO_SRC_0].clk_khz;
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2)
	clk1_khz = s_src[VIDEO_SRC_1].clk_khz;
#else
	clk1_khz = clk0_khz;
#endif
	if ((clk0_khz == 0U) || (clk1_khz == 0U))
	{
		xil_printf("video_inject: invalid clk\r\n");
		return XST_FAILURE;
	}
	/* Skip MMCM load when both rates already applied  -  peer stays glitch-free. */
	return clkwiz_set_dual_out_khz_if_changed(clk0_khz, clk1_khz,
						  did_reconfig);
}

static int inject_apply_path_ch(u8 ch, u8 light)
{
	int Status;

	Status = inject_apply_vtc_ch(ch);
	if (Status != XST_SUCCESS)
	{
		xil_printf("video_inject: vtc ch%u failed\r\n", (unsigned)ch);
		return Status;
	}
	Status = inject_apply_src(ch, &s_src[ch], light);
	if (Status != XST_SUCCESS)
	{
		xil_printf("video_inject: src ch%u failed\r\n", (unsigned)ch);
		return Status;
	}
	Status = inject_apply_csi_ch(ch);
	if (Status != XST_SUCCESS)
	{
		xil_printf("video_inject: csi ch%u failed\r\n", (unsigned)ch);
		return Status;
	}
	s_prev_w[ch] = s_src[ch].width;
	s_prev_h[ch] = s_src[ch].height;
	if (light != 0U)
	{
		xil_printf("video_inject: ch%u light recover %ux%u@%u\r\n",
			   (unsigned)ch,
			   (unsigned)s_src[ch].width,
			   (unsigned)s_src[ch].height,
			   (unsigned)s_src[ch].fps);
	}
	else
	{
		xil_printf("video_inject: ch%u=%ux%u@%u clk=%u%s\r\n",
			   (unsigned)ch,
			   (unsigned)s_src[ch].width,
			   (unsigned)s_src[ch].height,
			   (unsigned)s_src[ch].fps,
			   (unsigned)s_src[ch].clk_khz,
			   (s_src[ch].is_custom != 0U) ? " custom" : "");
	}
	return XST_SUCCESS;
}

static int inject_apply_vtc_ch(u8 ch)
{
#if defined (XPAR_XVTC_NUM_INSTANCES)
	vres_timing_t timing;

	video_cfg_to_vres(&s_src[ch], &timing);
	if (ch == VIDEO_SRC_0)
	{
		vtiming_gen_run_timing(&VtcInst0, &timing, 0);
		return XST_SUCCESS;
	}
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2) && (XPAR_XVTC_NUM_INSTANCES >= 2)
	if (ch == VIDEO_SRC_1)
	{
		vtiming_gen_run_timing(&VtcInst1, &timing, 0);
		return XST_SUCCESS;
	}
#endif
	return XST_FAILURE;
#else
	(void)ch;
	return XST_SUCCESS;
#endif
}

static int inject_apply_csi_ch(u8 ch)
{
#if defined (XPAR_XCSI2TX_NUM_INSTANCES)
	u16 width;

	/* HW: ch0 -> CSI0/1/2; CSI3 WC = mux source (PathA if same else PathB). */
	if (ch >= (u8)VIDEO_SRC_COUNT)
	{
		return XST_FAILURE;
	}

	if (ch == VIDEO_SRC_0)
	{
		width = s_src[VIDEO_SRC_0].width;
	}
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2)
	else if (ch == VIDEO_SRC_1)
	{
		width = inject_csi3_wc_width();
	}
#endif
	else
	{
		return XST_FAILURE;
	}

	/* Skip only when this CSI path already has the same WC. */
	if ((s_csi_w_valid[ch] != 0U) && (s_csi_w[ch] == width))
	{
		return XST_SUCCESS;
	}

	if (ch == VIDEO_SRC_0)
	{
		if (csi_tx_set_dt_wc(XPAR_ST_0_AXI_GPIO_0_DEVICE_ID,
				     (u32)width) != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 1U)
		XCsi2TxSs_Activate(&Csi2TxSsInst_0, XCSI2TX_ENABLE);
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 2U)
		XCsi2TxSs_Activate(&Csi2TxSsInst_1, XCSI2TX_ENABLE);
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 3U)
		XCsi2TxSs_Activate(&Csi2TxSsInst_2, XCSI2TX_ENABLE);
#endif
		s_csi_w[ch] = width;
		s_csi_w_valid[ch] = 1U;
		return XST_SUCCESS;
	}
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2)
	if (ch == VIDEO_SRC_1)
	{
		if (csi_tx_set_dt_wc(XPAR_ST_1_AXI_GPIO_0_DEVICE_ID,
				     (u32)width) != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 4U)
		XCsi2TxSs_Activate(&Csi2TxSsInst_3, XCSI2TX_ENABLE);
#endif
		s_csi_w[ch] = width;
		s_csi_w_valid[ch] = 1U;
		return XST_SUCCESS;
	}
#endif
	return XST_FAILURE;
#else
	(void)ch;
	return XST_SUCCESS;
#endif
}

/*
 * Apply one channel only when MMCM did not reload.
 * If MMCM reloaded (single shared wiz), reprogram BOTH paths without pipe reset
 * so the peer recovers after the unavoidable clock glitch.
 */
static int video_inject_apply_ch(u8 ch)
{
	int Status;
	u8 did_reconfig = 0U;
	u8 i;

	if (ch >= (u8)VIDEO_SRC_COUNT)
	{
		return XST_FAILURE;
	}

	Status = inject_clkwiz_apply_current(&did_reconfig);
	if (Status != XST_SUCCESS)
	{
		xil_printf("video_inject: clock failed\r\n");
		return Status;
	}

	if (did_reconfig != 0U)
	{
		usleep(2 * 1000);
		/* Mux first, then VTC/TPG - no mid-stream hard cut. */
		xil_printf("video_inject: mmcm reconfig, target full + peer light\r\n");
		if (inject_csi3_route_update() != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
		Status = inject_apply_path_ch(ch, 0U);
		if (Status != XST_SUCCESS)
		{
			return Status;
		}
		for (i = 0U; i < (u8)VIDEO_SRC_COUNT; i++)
		{
			if (i == ch)
			{
				continue;
			}
			Status = inject_apply_path_ch(i, 1U);
			if (Status != XST_SUCCESS)
			{
				return Status;
			}
		}
		s_have_geom = 1U;
		return XST_SUCCESS;
	}

	/* Clock rates unchanged: mux then VTC/TPG. */
	usleep(1 * 1000);
	if (inject_csi3_route_update() != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	Status = inject_apply_path_ch(ch, 0U);
	if (Status != XST_SUCCESS)
	{
		return Status;
	}
	s_have_geom = 1U;
	return XST_SUCCESS;
}

int video_inject_apply(void)
{
	int Status;
	u8 ch;
	u8 did_reconfig = 0U;

	/*
	 * Boot / factory: program both channels. Still skip MMCM if clocks
	 * already match applied shadow.
	 */
	if ((s_have_geom == 0U) || (inject_need_pipe_reset_any() != 0U))
	{
		xil_printf("video_inject: tpg_pipe_reset\r\n");
		tpg_pipe_reset_pulse();
		usleep(1 * 1000);
		/* Pipe reset / resolution change: force CSI WC rewrite. */
		inject_csi_shadow_invalidate();
	}

	Status = inject_clkwiz_apply_current(&did_reconfig);
	if (Status != XST_SUCCESS)
	{
		xil_printf("video_inject: clock failed\r\n");
		return Status;
	}
	usleep((did_reconfig != 0U) ? (2U * 1000U) : (1U * 1000U));

	/*
	 * Mux first, then VTC/TPG:
	 * same -> PathA then both VTCs; different -> PathB then both VTCs.
	 */
	if (inject_csi3_route_update() != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	for (ch = 0U; ch < (u8)VIDEO_SRC_COUNT; ch++)
	{
		Status = inject_apply_path_ch(ch, 0U);
		if (Status != XST_SUCCESS)
		{
			return Status;
		}
	}
	s_have_geom = 1U;
	return XST_SUCCESS;
}

int video_src_set_timing(u8 ch, u16 width, u16 height, u8 fps,
			 u16 h_fp, u16 h_sw, u16 h_bp, u8 h_pol,
			 u16 v_fp, u16 v_sw, u16 v_bp, u8 v_pol)
{
	video_src_cfg_t cfg;
	video_src_cfg_t prev;
	int Status;
	int Restore;

	if (ch >= (u8)VIDEO_SRC_COUNT)
	{
		xil_printf("video_src_set: bad ch %u\r\n", (unsigned)ch);
		return XST_FAILURE;
	}
	if ((fps == 0U) ||
	    (width <= VIDEO_MIN_ACTIVE) || (height <= VIDEO_MIN_ACTIVE) ||
	    ((width & 1U) != 0U))
	{
		xil_printf("video_src_set: reject %ux%u@%u\r\n",
			   (unsigned)width, (unsigned)height, (unsigned)fps);
		return XST_FAILURE;
	}

	/* Host supplies blanking; may stretch HBP so snapped pclk still meets fps. */
	Status = video_cfg_from_blanking(&cfg, width, height, fps, 1U, 0U,
					 (u32)h_fp, (u32)h_sw, (u32)h_bp,
					 (u32)h_pol,
					 (u32)v_fp, (u32)v_sw, (u32)v_bp,
					 (u32)v_pol);
	if (Status != XST_SUCCESS)
	{
		xil_printf("video_src_set: timing reject ch%u pclk=%u\r\n",
			   (unsigned)ch, (unsigned)cfg.clk_khz);
		return Status;
	}

	if ((s_have_geom != 0U) && (video_cfg_same_geom(&s_src[ch], &cfg) != 0U))
	{
		return XST_SUCCESS;
	}

	prev = s_src[ch];
	s_src[ch] = cfg;
	/*
	 * Isolation policy:
	 * - First bring-up / W/H change: shared pipe reset -> must reprogram BOTH.
	 * - fps/blanking/pclk-only: touch only this channel (skip MMCM if rates same).
	 */
	if ((s_have_geom == 0U) || (inject_need_pipe_reset_ch(ch) != 0U))
	{
		Status = video_inject_apply();
	}
	else
	{
		Status = video_inject_apply_ch(ch);
	}
	if (Status == XST_SUCCESS)
	{
		if (video_wait_ready_all() == XST_SUCCESS)
		{
			s_nv_dirty = 1U;
			return XST_SUCCESS;
		}
		xil_printf("video_src_set: mon ready fail, restoring ch%u\r\n",
			   (unsigned)ch);
	}

	/*
	 * Wait failed or apply failed: roll soft state back and force a full
	 * pipe bring-up. Previously restore used apply_ch without re-wait;
	 * a half-dead MMCM/VTC left the board NACKing every later set.
	 */
	s_src[ch] = prev;
	xil_printf("video_src_set: apply failed, hard-restore ch%u\r\n",
		   (unsigned)ch);
	tpg_pipe_reset_pulse();
	usleep(1 * 1000U);
	inject_csi_shadow_invalidate();
	Restore = video_inject_apply();
	if (Restore != XST_SUCCESS)
	{
		xil_printf("video_src_set: restore apply failed\r\n");
		return XST_FAILURE;
	}
	if (video_wait_ready_all() != XST_SUCCESS)
	{
		xil_printf("video_src_set: restore wait still fail\r\n");
		video_wait_dump_all("restore-fail");
		return XST_FAILURE;
	}
	xil_printf("video_src_set: restore recovered\r\n");
	/* Still report failure for the requested set (host may retry). */
	return XST_FAILURE;
}

int video_src_get_measured(u8 ch, u16 *width, u16 *height, u8 *fps)
{
#if defined (XPAR_TPG_0_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR)
	u32 base;
	u32 w;
	u32 h;
	u32 f;

	if ((ch >= (u8)VIDEO_SRC_COUNT) ||
	    (width == NULL) || (height == NULL) || (fps == NULL))
	{
		return XST_FAILURE;
	}
	base = video_tpg_mon_base(ch);
	if (base == 0U)
	{
		return XST_FAILURE;
	}
	if (video_mon_read(base, &w, &h, &f) == 0)
	{
		return XST_FAILURE;
	}
	if ((w > 65535U) || (h > 65535U) || (f > 255U))
	{
		return XST_FAILURE;
	}
	*width = (u16)w;
	*height = (u16)h;
	*fps = (u8)f;
	return XST_SUCCESS;
#else
	(void)ch;
	(void)width;
	(void)height;
	(void)fps;
	return XST_FAILURE;
#endif
}

static int video_inject_load_geom_nv(void)
{
	video_src_cfg_t nv[VIDEO_GEOM_NV_CH_MAX];
	u8 ch;
	u8 nch = (u8)VIDEO_SRC_COUNT;

	if (nch > VIDEO_GEOM_NV_CH_MAX)
	{
		nch = VIDEO_GEOM_NV_CH_MAX;
	}
	if (video_geom_nv_load(nv, nch) != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	for (ch = 0U; ch < nch; ch++)
	{
		if (video_cfg_from_blanking(
			    &s_src[ch],
			    nv[ch].width, nv[ch].height, nv[ch].fps,
			    1U, 1U, /* lock blanking from EEPROM */
			    nv[ch].h_fp, nv[ch].h_sw, nv[ch].h_bp, nv[ch].h_pol,
			    nv[ch].v_fp, nv[ch].v_sw, nv[ch].v_bp,
			    nv[ch].v_pol) != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
	}
	for (; ch < (u8)VIDEO_SRC_COUNT; ch++)
	{
		if (video_timing_resolve(VIDEO_DEFAULT_W, VIDEO_DEFAULT_H,
					 VIDEO_DEFAULT_FPS,
					 &s_src[ch]) != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
	}
	return XST_SUCCESS;
}

/*
 * Boot: VEM2 EEPROM -> s_src, else factory 1920x1080@30.
 * TPG pattern shadow was loaded in tpg_config(); HW starts here once.
 */
int video_inject_apply_default(void)
{
	int Status;

	if (video_inject_load_geom_nv() == XST_SUCCESS)
	{
		s_have_geom = 0U;
		Status = video_inject_apply();
		if (Status == XST_SUCCESS)
		{
			xil_printf("video_inject: geom from EEPROM\r\n");
			s_nv_dirty = 0U;
			(void)video_wait_ready_all();
			return XST_SUCCESS;
		}
	}
	else
	{
		xil_printf("video_inject: factory geom\r\n");
	}
	if (video_inject_fill_factory() != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	s_have_geom = 0U;
	Status = video_inject_apply();
	if (Status == XST_SUCCESS)
	{
		s_nv_dirty = 1U;
		(void)video_wait_ready_all();
	}
	return Status;
}

int video_inject_factory_default(void)
{
	int Status;

	Status = video_inject_fill_factory();
	if (Status != XST_SUCCESS)
	{
		return Status;
	}
#if defined (XPAR_XV_TPG_NUM_INSTANCES)
	/* Reset pattern shadow before apply so HW matches factory defaults. */
	tpg_pat_reset_defaults();
#endif
	s_have_geom = 0U;
	Status = video_inject_apply();
	if (Status != XST_SUCCESS)
	{
		return Status;
	}
	s_nv_dirty = 1U;
	(void)video_wait_ready_all();
	/* Always persist both regions; clear geom dirty only if all writes OK. */
	Status = video_geom_nv_save();
	if (Status != XST_SUCCESS)
	{
		return Status;
	}
#if defined (XPAR_XV_TPG_NUM_INSTANCES)
	Status = tpg_nv_save();
	if (Status != XST_SUCCESS)
	{
		xil_printf("video_nv: factory tpg save fail, geom dirty kept\r\n");
		return Status;
	}
#endif
	s_nv_dirty = 0U;
	return XST_SUCCESS;
}

#else /* no TPG */

const video_src_cfg_t *video_src_cfg_get(u8 ch)
{
	(void)ch;
	return NULL;
}

u8 video_src_host_count(void)
{
	return 0U;
}

u8 video_src_source_type(u8 idx)
{
	(void)idx;
	return VIDEO_CH_SRC_NONE;
}

int video_src_set_timing(u8 ch, u16 width, u16 height, u8 fps,
			 u16 h_fp, u16 h_sw, u16 h_bp, u8 h_pol,
			 u16 v_fp, u16 v_sw, u16 v_bp, u8 v_pol)
{
	(void)ch;
	(void)width;
	(void)height;
	(void)fps;
	(void)h_fp;
	(void)h_sw;
	(void)h_bp;
	(void)h_pol;
	(void)v_fp;
	(void)v_sw;
	(void)v_bp;
	(void)v_pol;
	return XST_FAILURE;
}

int video_src_get_measured(u8 ch, u16 *width, u16 *height, u8 *fps)
{
	(void)ch;
	(void)width;
	(void)height;
	(void)fps;
	return XST_FAILURE;
}

int video_inject_apply(void)
{
	return XST_FAILURE;
}

int video_inject_apply_default(void)
{
	return XST_FAILURE;
}

int video_inject_save_nv(void)
{
	return XST_FAILURE;
}

int video_inject_factory_default(void)
{
	return XST_FAILURE;
}

#endif

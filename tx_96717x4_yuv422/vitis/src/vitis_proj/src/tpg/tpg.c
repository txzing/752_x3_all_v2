#include "../bsp.h"
#include "xgpio.h"
#if defined (XPAR_XV_TPG_NUM_INSTANCES)

#if (XPAR_XV_TPG_NUM_INSTANCES >= 1U)
XV_tpg tpg_inst0;
static XVidC_ColorFormat s_color_fmt0 = XVIDC_CSF_RGB;
#endif
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
XV_tpg tpg_inst1;
static XVidC_ColorFormat s_color_fmt1 = XVIDC_CSF_RGB;
#endif

/* VTP1: 8B hdr + nch*8 must fit EEPROM_TPG_PAT_BYTES (32 => max 3 ch). */
#define TPG_NV_CH_BYTES      8U
#define TPG_NV_BYTES         EEPROM_TPG_PAT_BYTES
#define TPG_PAT_CH_MAX       XPAR_XV_TPG_NUM_INSTANCES
#define TPG_PAT_BOX_SIZE_DEF 50U
#define TPG_PAT_SPEED_DEF    1U

#define TPG_BOX_OFF     0U
#define TPG_BOX_STATIC  1U
#define TPG_BOX_MOTION  2U

#define TPG_NV_MAGIC0   0x56U /* V */
#define TPG_NV_MAGIC1   0x54U /* T */
#define TPG_NV_MAGIC2   0x50U /* P */
#define TPG_NV_MAGIC3   0x31U /* 1 */
#define TPG_NV_VER      1U

/* Preprocessor cannot use casts; 32B region fits at most 3 pattern slots. */
#if XPAR_XV_TPG_NUM_INSTANCES > 3
#error "TPG VTP1: channel count exceeds EEPROM_TPG_PAT_BYTES"
#endif

typedef struct {
	u8 bckgnd;
	u8 box_mode;
	u8 motion_speed;
	u8 box_size;
	u8 box_r;
	u8 box_g;
	u8 box_b;
} tpg_pat_cfg_t;

static tpg_pat_cfg_t s_pat[TPG_PAT_CH_MAX];
static u8 s_pat_nv_dirty;

/* ---------- shadow defaults / validation ---------- */

static void tpg_pat_default(tpg_pat_cfg_t *p)
{
	p->bckgnd = (u8)XTPG_BKGND_SOLID_RED;
	p->box_mode = TPG_BOX_MOTION;
	p->motion_speed = TPG_PAT_SPEED_DEF;
	p->box_size = TPG_PAT_BOX_SIZE_DEF;
	p->box_r = 235U;
	p->box_g = 20U;
	p->box_b = 20U;
}

static int tpg_pat_valid(const tpg_pat_cfg_t *p)
{
	if ((p->bckgnd == 0U) || (p->bckgnd >= (u8)XTPG_BKGND_LAST) ||
	    (p->box_mode > TPG_BOX_MOTION))
	{
		return 0;
	}
	if (p->box_mode == TPG_BOX_OFF)
	{
		return 1;
	}
	if (p->box_size == 0U)
	{
		return 0;
	}
	if (p->box_mode == TPG_BOX_MOTION)
	{
		return (p->motion_speed != 0U) ? 1 : 0;
	}
	return 1;
}

static u8 tpg_pat_ch_count(void)
{
	u8 n = (u8)VIDEO_SRC_COUNT;

	return (n > TPG_PAT_CH_MAX) ? TPG_PAT_CH_MAX : n;
}

/* ---------- HW helpers ---------- */

XV_tpg *tpg_get_instance(u8 ch)
{
	if (ch == VIDEO_SRC_0)
	{
#if (XPAR_XV_TPG_NUM_INSTANCES >= 1U)
		return &tpg_inst0;
#endif
	}
	else if (ch == VIDEO_SRC_1)
	{
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
		return &tpg_inst1;
#endif
	}
	return NULL;
}

static u32 tpg_color_fmt(u8 ch)
{
	if (ch == VIDEO_SRC_0)
	{
#if (XPAR_XV_TPG_NUM_INSTANCES >= 1U)
		return (u32)s_color_fmt0;
#endif
	}
	else if (ch == VIDEO_SRC_1)
	{
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
		return (u32)s_color_fmt1;
#endif
	}
	return (u32)XVIDC_CSF_RGB;
}

static void tpg_hw_apply_box(XV_tpg *inst, const tpg_pat_cfg_t *p)
{
	if (p->box_mode == TPG_BOX_OFF)
	{
		XV_tpg_Set_motionSpeed(inst, 0U);
		XV_tpg_Set_ovrlayId(inst, 0U);
		return;
	}
	XV_tpg_Set_boxColorR(inst, (u32)p->box_r);
	XV_tpg_Set_boxColorG(inst, (u32)p->box_g);
	XV_tpg_Set_boxColorB(inst, (u32)p->box_b);
	XV_tpg_Set_boxSize(inst, (u32)p->box_size);
	if (p->box_mode == TPG_BOX_STATIC)
	{
		XV_tpg_Set_motionSpeed(inst, 0U);
	}
	else
	{
		XV_tpg_Set_motionSpeed(inst, (u32)p->motion_speed);
	}
	XV_tpg_Set_ovrlayId(inst, 1U);
}

static int tpg_hw_apply(u8 ch, u32 height, u32 width)
{
	XV_tpg *inst = tpg_get_instance(ch);
	const tpg_pat_cfg_t *p;

	if ((inst == NULL) || (ch >= TPG_PAT_CH_MAX))
	{
		return XST_FAILURE;
	}
	p = &s_pat[ch];
	XV_tpg_DisableAutoRestart(inst);
	XV_tpg_Set_height(inst, height);
	XV_tpg_Set_width(inst, width);
	XV_tpg_Set_colorFormat(inst, tpg_color_fmt(ch));
	XV_tpg_Set_bckgndId(inst, (u32)p->bckgnd);
	tpg_hw_apply_box(inst, p);
	XV_tpg_EnableAutoRestart(inst);
	XV_tpg_Start(inst);
	return XST_SUCCESS;
}

/* ---------- pipe reset GPIO ---------- */

#if defined (XPAR_AXI_GPIO_RESET_DEVICE_ID)
static XGpio s_tpg_rst_gpio;
static u8 s_tpg_rst_ready;

int tpg_rst_gpio_init(void)
{
	int Status;

	if (s_tpg_rst_ready != 0U)
	{
		XGpio_DiscreteWrite(&s_tpg_rst_gpio, 1, 0x1);
		return XST_SUCCESS;
	}
	Status = XGpio_Initialize(&s_tpg_rst_gpio, XPAR_AXI_GPIO_RESET_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		return Status;
	}
	XGpio_SetDataDirection(&s_tpg_rst_gpio, 1, 0x0);
	XGpio_DiscreteWrite(&s_tpg_rst_gpio, 1, 0x1);
	s_tpg_rst_ready = 1U;
	usleep(200);
	return XST_SUCCESS;
}

void tpg_pipe_reset_pulse(void)
{
	if (s_tpg_rst_ready == 0U)
	{
		(void)tpg_rst_gpio_init();
	}
	XGpio_DiscreteWrite(&s_tpg_rst_gpio, 1, 0x0);
	XGpio_DiscreteWrite(&s_tpg_rst_gpio, 1, 0x1);
	usleep(200);
#if defined (XPAR_XVPROCSS_NUM_INSTANCES)
	vpss_invalidate();
#endif
}
#else
int tpg_rst_gpio_init(void)
{
	return XST_SUCCESS;
}

void tpg_pipe_reset_pulse(void)
{
}
#endif

/* ---------- public API ---------- */

int tpg_apply_geometry(u8 ch, u32 height, u32 width)
{
	return tpg_hw_apply(ch, height, width);
}

void tpg_pat_reset_defaults(void)
{
	u8 ch;

	for (ch = 0U; ch < TPG_PAT_CH_MAX; ch++)
	{
		tpg_pat_default(&s_pat[ch]);
	}
	s_pat_nv_dirty = 1U;
}

int tpg_set_bckgnd(u8 ch, u32 bckgndId)
{
	XV_tpg *inst = tpg_get_instance(ch);

	if ((inst == NULL) || (ch >= TPG_PAT_CH_MAX) ||
	    (bckgndId == 0U) || (bckgndId >= (u32)XTPG_BKGND_LAST))
	{
		return XST_FAILURE;
	}
	s_pat[ch].bckgnd = (u8)bckgndId;
	s_pat_nv_dirty = 1U;
	XV_tpg_Set_bckgndId(inst, bckgndId);
	return XST_SUCCESS;
}

int tpg_set_box_motion(u8 ch, u8 mode, u8 motion_speed)
{
	XV_tpg *inst = tpg_get_instance(ch);
	u8 speed = (motion_speed == 0U) ? TPG_PAT_SPEED_DEF : motion_speed;

	if ((inst == NULL) || (ch >= TPG_PAT_CH_MAX) || (mode > TPG_BOX_MOTION))
	{
		return XST_FAILURE;
	}
	s_pat[ch].box_mode = mode;
	if (mode == TPG_BOX_MOTION)
	{
		s_pat[ch].motion_speed = speed;
	}
	else if (mode == TPG_BOX_STATIC)
	{
		s_pat[ch].motion_speed = 0U;
	}
	s_pat_nv_dirty = 1U;
	tpg_hw_apply_box(inst, &s_pat[ch]);
	return XST_SUCCESS;
}

int tpg_set_box_color(u8 ch, u8 r, u8 g, u8 b)
{
	XV_tpg *inst = tpg_get_instance(ch);

	if ((inst == NULL) || (ch >= TPG_PAT_CH_MAX))
	{
		return XST_FAILURE;
	}
	s_pat[ch].box_r = r;
	s_pat[ch].box_g = g;
	s_pat[ch].box_b = b;
	s_pat_nv_dirty = 1U;
	XV_tpg_Set_boxColorR(inst, (u32)r);
	XV_tpg_Set_boxColorG(inst, (u32)g);
	XV_tpg_Set_boxColorB(inst, (u32)b);
	return XST_SUCCESS;
}

u8 tpg_nv_is_dirty(void)
{
	return s_pat_nv_dirty;
}

/* ---------- EEPROM VTP1 ---------- */

static u8 tpg_nv_csum(const u8 *p, u16 n)
{
	u16 i;
	u8 calc = 0U;

	for (i = 0U; i < n; i++)
	{
		calc = (u8)(calc + p[i]);
	}
	return (u8)(~calc & 0xFFU);
}

static void tpg_nv_put_ch(u8 *slot, const tpg_pat_cfg_t *p)
{
	slot[0] = p->bckgnd;
	slot[1] = p->box_mode;
	slot[2] = p->motion_speed;
	slot[3] = p->box_size;
	slot[4] = p->box_r;
	slot[5] = p->box_g;
	slot[6] = p->box_b;
	slot[7] = 0U;
}

static void tpg_nv_get_ch(const u8 *slot, tpg_pat_cfg_t *p)
{
	p->bckgnd = slot[0];
	p->box_mode = slot[1];
	p->motion_speed = slot[2];
	p->box_size = slot[3];
	p->box_r = slot[4];
	p->box_g = slot[5];
	p->box_b = slot[6];
}

static int tpg_nv_load(void)
{
#if defined (USING_EEPROM) && defined (__EEPROM_H__)
	u8 buf[TPG_NV_BYTES];
	u8 ch;
	u8 nch = tpg_pat_ch_count();

	memset(buf, 0, sizeof(buf));
	if (EepromReadData(&ps_i2c_0, buf, TPG_NV_BYTES,
			   (u16)EEPROM_ADDR_TPG_PAT) != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	if ((buf[0] != tpg_nv_csum(&buf[1], (u16)(TPG_NV_BYTES - 1U))) ||
	    (buf[1] != TPG_NV_MAGIC0) || (buf[2] != TPG_NV_MAGIC1) ||
	    (buf[3] != TPG_NV_MAGIC2) || (buf[4] != TPG_NV_MAGIC3) ||
	    (buf[5] != TPG_NV_VER) || (buf[6] < nch))
	{
		return XST_FAILURE;
	}
	for (ch = 0U; ch < nch; ch++)
	{
		tpg_pat_cfg_t tmp;

		tpg_nv_get_ch(&buf[8U + (ch * TPG_NV_CH_BYTES)], &tmp);
		if (tpg_pat_valid(&tmp) != 0)
		{
			s_pat[ch] = tmp;
		}
		else
		{
			tpg_pat_default(&s_pat[ch]);
		}
	}
	s_pat_nv_dirty = 0U;
	xil_printf("tpg_nv: loaded from EEPROM\r\n");
	return XST_SUCCESS;
#else
	return XST_FAILURE;
#endif
}

int tpg_nv_save(void)
{
#if defined (USING_EEPROM) && defined (__EEPROM_H__)
	u8 buf[TPG_NV_BYTES];
	u8 ch;
	u8 nch = tpg_pat_ch_count();

	memset(buf, 0, sizeof(buf));
	buf[1] = TPG_NV_MAGIC0;
	buf[2] = TPG_NV_MAGIC1;
	buf[3] = TPG_NV_MAGIC2;
	buf[4] = TPG_NV_MAGIC3;
	buf[5] = TPG_NV_VER;
	buf[6] = nch;
	for (ch = 0U; ch < nch; ch++)
	{
		tpg_nv_put_ch(&buf[8U + (ch * TPG_NV_CH_BYTES)], &s_pat[ch]);
	}
	buf[0] = tpg_nv_csum(&buf[1], (u16)(TPG_NV_BYTES - 1U));
	if (EepromWriteVerify(&ps_i2c_0, buf, TPG_NV_BYTES,
			      (u16)EEPROM_ADDR_TPG_PAT) != XST_SUCCESS)
	{
		xil_printf("tpg_nv: save/verify failed\r\n");
		return XST_FAILURE;
	}
	s_pat_nv_dirty = 0U;
	xil_printf("tpg_nv: saved\r\n");
	return XST_SUCCESS;
#else
	return XST_FAILURE;
#endif
}

/*
 * Boot (same idea as eth loadconfig):
 *   1) factory pattern shadow
 *   2) EEPROM VTP1 overrides if valid
 *   3) Initialize TPG driver only; first HW start is in video_inject_apply_default()
 */
int tpg_config(void)
{
	u32 Status;
	u8 ch;

	if (tpg_rst_gpio_init() != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	for (ch = 0U; ch < TPG_PAT_CH_MAX; ch++)
	{
		tpg_pat_default(&s_pat[ch]);
	}
	s_pat_nv_dirty = 0U;
	if (tpg_nv_load() != XST_SUCCESS)
	{
		xil_printf("tpg_nv: use factory pattern\r\n");
	}

#if (XPAR_XV_TPG_NUM_INSTANCES >= 1U)
	Status = XV_tpg_Initialize(&tpg_inst0, XPAR_XV_TPG_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
#endif
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
	Status = XV_tpg_Initialize(&tpg_inst1, XPAR_XV_TPG_1_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
#endif
	return XST_SUCCESS;
}

#endif /* XPAR_XV_TPG_NUM_INSTANCES */

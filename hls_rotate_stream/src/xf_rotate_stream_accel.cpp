/*
 * rotate_stream_accel — realtime AXIS pipeline (DATAFLOW + ping-pong DDR).
 *
 * Three concurrent stages share gmem ping-pong slots via descriptors:
 *   capture_axis -> rotate_ddr -> emit_axis
 * Software programs height/width/direction/fb once and uses auto_restart /
 * continuous invocation so the pipeline stays filled.
 */

#include "xf_rotate_stream_config.h"
#include "rotate_kernel.hpp"

#include <stddef.h>

#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

typedef ap_axiu<ROTATE_AXIS_W, 1, 1, 1> video_axis_t;

struct frame_desc {
    ap_uint<1> in_sel;
    ap_uint<1> out_sel;
    int height;
    int width;
    int direction;
    ap_uint<32> stride;
};

static void capture_axis(hls::stream<video_axis_t>& s_axis,
                         ap_uint<8>* gmem,
                         ap_uint<1> in_sel,
                         int height,
                         int width,
                         ap_uint<32> stride,
                         hls::stream<frame_desc>& to_rotate,
                         int direction,
                         ap_uint<1> out_sel) {
#pragma HLS INLINE off
    unsigned char* fb = (unsigned char*)(gmem + (in_sel ? stride : (ap_uint<32>)0));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II = 1
            video_axis_t pix = s_axis.read();
            unsigned char* p = fb + ((size_t)y * (size_t)width + (size_t)x) * 3u;
            p[0] = (unsigned char)pix.data.range(7, 0);
            p[1] = (unsigned char)pix.data.range(15, 8);
            p[2] = (unsigned char)pix.data.range(23, 16);
            (void)pix.user;
            (void)pix.last;
        }
    }
    frame_desc d;
    d.in_sel = in_sel;
    d.out_sel = out_sel;
    d.height = height;
    d.width = width;
    d.direction = direction;
    d.stride = stride;
    to_rotate.write(d);
}

static void rotate_ddr(ap_uint<8>* gmem, hls::stream<frame_desc>& from_cap, hls::stream<frame_desc>& to_emit) {
#pragma HLS INLINE off
    frame_desc d = from_cap.read();
    unsigned char* in_fb = (unsigned char*)(gmem + (d.in_sel ? d.stride : (ap_uint<32>)0));
    unsigned char* out_fb =
        (unsigned char*)(gmem + ((ap_uint<32>)2 * d.stride) + (d.out_sel ? d.stride : (ap_uint<32>)0));
    rotate_frame_rgb888(in_fb, out_fb, d.height, d.width, d.direction);
    to_emit.write(d);
}

static void emit_axis(hls::stream<video_axis_t>& m_axis, ap_uint<8>* gmem, hls::stream<frame_desc>& from_rot) {
#pragma HLS INLINE off
    frame_desc d = from_rot.read();
    int out_h = 0, out_w = 0;
    rotate_get_out_size(d.height, d.width, d.direction, &out_h, &out_w);
    unsigned char* fb =
        (unsigned char*)(gmem + ((ap_uint<32>)2 * d.stride) + (d.out_sel ? d.stride : (ap_uint<32>)0));
    for (int y = 0; y < out_h; ++y) {
        for (int x = 0; x < out_w; ++x) {
#pragma HLS PIPELINE II = 1
            const unsigned char* p = fb + ((size_t)y * (size_t)out_w + (size_t)x) * 3u;
            video_axis_t pix;
            pix.data.range(7, 0) = p[0];
            pix.data.range(15, 8) = p[1];
            pix.data.range(23, 16) = p[2];
            pix.keep = -1;
            pix.strb = -1;
            pix.user = (y == 0 && x == 0) ? 1 : 0;
            pix.last = (x == out_w - 1) ? 1 : 0;
            pix.id = 0;
            pix.dest = 0;
            m_axis.write(pix);
        }
    }
}

void rotate_stream_accel(hls::stream<video_axis_t>& s_axis_video,
                         hls::stream<video_axis_t>& m_axis_video,
                         ap_uint<8>* gmem,
                         int height,
                         int width,
                         int direction,
                         ap_uint<32> fb_bytes) {
// clang-format off
#pragma HLS INTERFACE axis port=s_axis_video
#pragma HLS INTERFACE axis port=m_axis_video
#pragma HLS INTERFACE m_axi port=gmem offset=slave bundle=gmem depth=88473600 num_read_outstanding=32 num_write_outstanding=32 max_read_burst_length=256 max_write_burst_length=256
#pragma HLS INTERFACE s_axilite port=height bundle=control
#pragma HLS INTERFACE s_axilite port=width bundle=control
#pragma HLS INTERFACE s_axilite port=direction bundle=control
#pragma HLS INTERFACE s_axilite port=fb_bytes bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
#pragma HLS DATAFLOW
    // clang-format on

    static ap_uint<1> wr_sel = 0;
    static ap_uint<1> rd_sel = 0;

    int out_h = 0, out_w = 0;
    rotate_get_out_size(height, width, direction, &out_h, &out_w);
    ap_uint<32> in_bytes = fb_bytes;
    ap_uint<32> out_bytes = (ap_uint<32>)out_h * (ap_uint<32>)out_w * ROTATE_BPP;
    ap_uint<32> stride = (in_bytes > out_bytes) ? in_bytes : out_bytes;

    hls::stream<frame_desc> cap2rot("cap2rot");
    hls::stream<frame_desc> rot2emit("rot2emit");
#pragma HLS STREAM variable = cap2rot depth = 2
#pragma HLS STREAM variable = rot2emit depth = 2

    ap_uint<1> in_sel = wr_sel;
    ap_uint<1> out_sel = rd_sel;
    wr_sel = ~wr_sel;
    rd_sel = ~rd_sel;

    capture_axis(s_axis_video, gmem, in_sel, height, width, stride, cap2rot, direction, out_sel);
    rotate_ddr(gmem, cap2rot, rot2emit);
    emit_axis(m_axis_video, gmem, rot2emit);
}

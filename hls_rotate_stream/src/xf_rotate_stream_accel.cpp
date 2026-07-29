/*
 * rotate_stream_accel — AXIS video in/out + DDR multi-buffer + frame-auto rotate.
 *
 * Software programs height/width/direction/fb_base once, asserts ap_start with
 * auto_restart (or repeatedly starts); each transaction consumes one input frame
 * and produces one rotated output frame. Ping-pong indices advance every frame.
 */

#include "xf_rotate_stream_config.h"
#include "rotate_kernel.hpp"

#include <stddef.h>

#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

typedef ap_axiu<ROTATE_AXIS_W, 1, 1, 1> video_axis_t;

static void capture_frame(hls::stream<video_axis_t>& s_axis,
                          unsigned char* fb,
                          int height,
                          int width) {
#pragma HLS INLINE off
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II = 1
            video_axis_t pix = s_axis.read();
            unsigned char* p = fb + ((size_t)y * (size_t)width + (size_t)x) * 3u;
            p[0] = (unsigned char)pix.data.range(7, 0);
            p[1] = (unsigned char)pix.data.range(15, 8);
            p[2] = (unsigned char)pix.data.range(23, 16);
            /* TUSER SOF on (0,0), TLAST on EOL — accepted, not checked every beat */
            (void)pix.user;
            (void)pix.last;
        }
    }
}

static void emit_frame(hls::stream<video_axis_t>& m_axis,
                       const unsigned char* fb,
                       int height,
                       int width) {
#pragma HLS INLINE off
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II = 1
            const unsigned char* p = fb + ((size_t)y * (size_t)width + (size_t)x) * 3u;
            video_axis_t pix;
            pix.data.range(7, 0) = p[0];
            pix.data.range(15, 8) = p[1];
            pix.data.range(23, 16) = p[2];
            pix.keep = -1;
            pix.strb = -1;
            pix.user = (y == 0 && x == 0) ? 1 : 0;
            pix.last = (x == width - 1) ? 1 : 0;
            pix.id = 0;
            pix.dest = 0;
            m_axis.write(pix);
        }
    }
}

/*
 * gmem is a contiguous DDR window holding 4 frame buffers:
 *   [0]=IN0, [1]=IN1, [2]=OUT0, [3]=OUT1
 * fb_bytes = height*width*3 (input frame size). Output uses out_h*out_w*3
 * which for 90/270 equals input byte count; for 180 same.
 */
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
#pragma HLS INTERFACE m_axi port=gmem offset=slave bundle=gmem depth=88473600 num_read_outstanding=16 num_write_outstanding=16 max_read_burst_length=64 max_write_burst_length=64
#pragma HLS INTERFACE s_axilite port=height
#pragma HLS INTERFACE s_axilite port=width
#pragma HLS INTERFACE s_axilite port=direction
#pragma HLS INTERFACE s_axilite port=fb_bytes
#pragma HLS INTERFACE s_axilite port=return
    // clang-format on

    /* Persistent ping-pong indices across auto-restart invocations */
    static ap_uint<1> wr_sel = 0;
    static ap_uint<1> rd_sel = 0;

    const ap_uint<32> in_bytes = fb_bytes;
    int out_h = 0;
    int out_w = 0;
    rotate_get_out_size(height, width, direction, &out_h, &out_w);
    const ap_uint<32> out_bytes = (ap_uint<32>)out_h * (ap_uint<32>)out_w * ROTATE_BPP;

    /* Layout: IN0 | IN1 | OUT0 | OUT1  (each sized to max of in/out bytes → use fb_bytes param as stride) */
    const ap_uint<32> stride = (in_bytes > out_bytes) ? in_bytes : out_bytes;
    unsigned char* in_fb = (unsigned char*)(gmem + (wr_sel ? stride : (ap_uint<32>)0));
    unsigned char* out_fb =
        (unsigned char*)(gmem + ((ap_uint<32>)2 * stride) + (rd_sel ? stride : (ap_uint<32>)0));

    capture_frame(s_axis_video, in_fb, height, width);
    rotate_frame_rgb888(in_fb, out_fb, height, width, direction);
    emit_frame(m_axis_video, out_fb, out_h, out_w);

    wr_sel = ~wr_sel;
    rd_sel = ~rd_sel;
}

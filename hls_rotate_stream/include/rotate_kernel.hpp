#ifndef ROTATE_KERNEL_HPP
#define ROTATE_KERNEL_HPP

/*
 * Self-contained RGB888 (1ppc) frame rotate for HLS / C-sim / reference.
 * direction: 0=90CW, 1=180, 2=270CW (same convention as Vitis Vision rotate).
 */

#include <stddef.h>
#include <stdint.h>

static inline void rotate_get_out_size(int in_h, int in_w, int direction, int* out_h, int* out_w) {
    if (direction == 1) {
        *out_h = in_h;
        *out_w = in_w;
    } else {
        *out_h = in_w;
        *out_w = in_h;
    }
}

/*
 * Rotate packed BGR/RGB interleaved buffer (3 bytes/pixel).
 * in:  in_h * in_w * 3
 * out: out_h * out_w * 3
 */
static inline void rotate_frame_rgb888(const unsigned char* in,
                                       unsigned char* out,
                                       int in_h,
                                       int in_w,
                                       int direction) {
    int out_h = 0;
    int out_w = 0;
    rotate_get_out_size(in_h, in_w, direction, &out_h, &out_w);

    for (int y = 0; y < out_h; ++y) {
        for (int x = 0; x < out_w; ++x) {
            int sx = 0;
            int sy = 0;
            if (direction == 0) {
                /* 90 CW: out(x,y) <- in(in_h-1-x, y) with out_w=in_h, out_h=in_w */
                sx = y;
                sy = in_h - 1 - x;
            } else if (direction == 1) {
                sx = in_w - 1 - x;
                sy = in_h - 1 - y;
            } else {
                /* 270 CW / 90 CCW */
                sx = in_w - 1 - y;
                sy = x;
            }
            const unsigned char* sp = in + ((size_t)sy * (size_t)in_w + (size_t)sx) * 3u;
            unsigned char* dp = out + ((size_t)y * (size_t)out_w + (size_t)x) * 3u;
            dp[0] = sp[0];
            dp[1] = sp[1];
            dp[2] = sp[2];
        }
    }
}

#ifdef __SYNTHESIS__
#include "ap_int.h"

/* HLS-friendly DDR rotate: word-addressed 32-bit packing of RGB888 stream in DRAM */
template <int MAX_H, int MAX_W>
void rotate_frame_m_axi(ap_uint<32>* gmem_in,
                        ap_uint<32>* gmem_out,
                        int in_h,
                        int in_w,
                        int direction) {
#pragma HLS INLINE off
    int out_h = 0;
    int out_w = 0;
    rotate_get_out_size(in_h, in_w, direction, &out_h, &out_w);

    /* Byte-wise via 32-bit DRAM words: store 3 bytes/pixel tightly in a byte array view.
     * For HLS C-sim we use a simpler path in the accel that copies to local buffers.
     * This stub keeps synthesis hooks; runtime path uses byte buffers in DDR window. */
    (void)gmem_in;
    (void)gmem_out;
    (void)MAX_H;
    (void)MAX_W;
    (void)out_h;
    (void)out_w;
}
#endif

#endif /* ROTATE_KERNEL_HPP */

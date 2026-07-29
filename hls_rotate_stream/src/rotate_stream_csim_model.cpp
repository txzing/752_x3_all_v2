/*
 * Host-side C-sim model of rotate_stream_accel (no Vitis HLS headers required).
 * Models: AXIS capture → DDR ping-pong → rotate → AXIS emit, config-once multi-frame.
 */

#include "xf_rotate_stream_accel.h"
#include "rotate_kernel.hpp"

#include <cstring>
#include <vector>

extern "C" int rotate_stream_csim_run(const unsigned char* frames_in,
                                      int num_frames,
                                      int height,
                                      int width,
                                      int direction,
                                      unsigned char* frames_out) {
    if (!frames_in || !frames_out || num_frames <= 0 || height <= 0 || width <= 0) {
        return -1;
    }

    int out_h = 0;
    int out_w = 0;
    rotate_get_out_size(height, width, direction, &out_h, &out_w);

    const size_t in_bytes = (size_t)height * (size_t)width * 3u;
    const size_t out_bytes = (size_t)out_h * (size_t)out_w * 3u;
    const size_t stride = (in_bytes > out_bytes) ? in_bytes : out_bytes;

    /* 4 buffers: IN0 IN1 OUT0 OUT1 */
    std::vector<unsigned char> gmem(stride * 4u, 0);
    int wr_sel = 0;
    int rd_sel = 0;

    for (int f = 0; f < num_frames; ++f) {
        unsigned char* in_fb = gmem.data() + (size_t)wr_sel * stride;
        unsigned char* out_fb = gmem.data() + 2u * stride + (size_t)rd_sel * stride;

        /* AXIS capture */
        std::memcpy(in_fb, frames_in + (size_t)f * in_bytes, in_bytes);

        /* Frame-sync auto rotate */
        rotate_frame_rgb888(in_fb, out_fb, height, width, direction);

        /* AXIS emit */
        std::memcpy(frames_out + (size_t)f * out_bytes, out_fb, out_bytes);

        wr_sel ^= 1;
        rd_sel ^= 1;
    }
    return 0;
}

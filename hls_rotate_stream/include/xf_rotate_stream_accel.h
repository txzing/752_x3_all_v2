#ifndef XF_ROTATE_STREAM_ACCEL_H
#define XF_ROTATE_STREAM_ACCEL_H

#include "xf_rotate_stream_config.h"

#ifdef __VITIS_HLS__
#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
typedef ap_axiu<ROTATE_AXIS_W, 1, 1, 1> video_axis_t;
void rotate_stream_accel(hls::stream<video_axis_t>& s_axis_video,
                         hls::stream<video_axis_t>& m_axis_video,
                         ap_uint<8>* gmem,
                         int height,
                         int width,
                         int direction,
                         ap_uint<32> fb_bytes);
#else
/* Host / g++ C-sim entry: feed packed RGB frames, get rotated frames */
#ifdef __cplusplus
extern "C" {
#endif
int rotate_stream_csim_run(const unsigned char* frames_in,
                           int num_frames,
                           int height,
                           int width,
                           int direction,
                           unsigned char* frames_out);
#ifdef __cplusplus
}
#endif
#endif

#endif /* XF_ROTATE_STREAM_ACCEL_H */

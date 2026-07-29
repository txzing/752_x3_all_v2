/*
 * AXIS TB for Vitis HLS cosim of rotate_stream_accel.
 * Configures once, streams N frames via hls::stream AXIS.
 */
#include "xf_rotate_stream_config.h"
#include "rotate_kernel.hpp"

#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

typedef ap_axiu<ROTATE_AXIS_W, 1, 1, 1> video_axis_t;

void rotate_stream_accel(hls::stream<video_axis_t>& s_axis_video,
                         hls::stream<video_axis_t>& m_axis_video,
                         ap_uint<8>* gmem,
                         int height,
                         int width,
                         int direction,
                         ap_uint<32> fb_bytes);

static bool load_bin(const std::string& path, std::vector<unsigned char>& buf, size_t expect) {
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) {
        fprintf(stderr, "ERROR: cannot open %s\n", path.c_str());
        return false;
    }
    buf.resize(expect);
    size_t n = fread(buf.data(), 1, expect, fp);
    fclose(fp);
    return n == expect;
}

int main(int argc, char** argv) {
    std::string dir = (argc >= 2) ? argv[1] : "tb_data";
    int direction = (argc >= 3) ? atoi(argv[2]) : 0;
    int num_frames = (argc >= 4) ? atoi(argv[3]) : 3;
    int height = (argc >= 5) ? atoi(argv[4]) : 32;
    int width = (argc >= 6) ? atoi(argv[5]) : 64;

    int out_h = 0, out_w = 0;
    rotate_get_out_size(height, width, direction, &out_h, &out_w);
    const size_t in_bytes = (size_t)height * (size_t)width * 3u;
    const size_t out_bytes = (size_t)out_h * (size_t)out_w * 3u;
    const size_t stride = (in_bytes > out_bytes) ? in_bytes : out_bytes;

    std::vector<unsigned char> din;
    if (!load_bin(dir + "/input_bgr.bin", din, in_bytes)) {
        /* generate if missing */
        din.resize(in_bytes);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                size_t o = ((size_t)y * width + x) * 3u;
                din[o + 0] = (unsigned char)((x * 3 + y * 5) & 0xFF);
                din[o + 1] = (unsigned char)((x * 7 + y * 11) & 0xFF);
                din[o + 2] = (unsigned char)((x * 13 + y * 17) & 0xFF);
            }
        }
    }

    std::vector<unsigned char> frames_in((size_t)num_frames * in_bytes);
    std::vector<unsigned char> gold((size_t)num_frames * out_bytes);
    for (int f = 0; f < num_frames; ++f) {
        for (size_t i = 0; i < in_bytes; ++i)
            frames_in[(size_t)f * in_bytes + i] = (unsigned char)(din[i] ^ (unsigned char)(f * 17));
        rotate_frame_rgb888(frames_in.data() + (size_t)f * in_bytes, gold.data() + (size_t)f * out_bytes,
                            height, width, direction);
    }

    std::vector<ap_uint<8>> gmem(stride * 4u, 0);
    std::vector<unsigned char> dout((size_t)num_frames * out_bytes, 0);

    printf("AXIS cosim TB: %d frames in %dx%d dir=%d\n", num_frames, width, height, direction);

    for (int f = 0; f < num_frames; ++f) {
        hls::stream<video_axis_t> s_axis("s_axis");
        hls::stream<video_axis_t> m_axis("m_axis");

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const unsigned char* p =
                    frames_in.data() + (size_t)f * in_bytes + ((size_t)y * width + x) * 3u;
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
                s_axis.write(pix);
            }
        }

        rotate_stream_accel(s_axis, m_axis, gmem.data(), height, width, direction, (ap_uint<32>)stride);

        for (int y = 0; y < out_h; ++y) {
            for (int x = 0; x < out_w; ++x) {
                video_axis_t pix = m_axis.read();
                unsigned char* p =
                    dout.data() + (size_t)f * out_bytes + ((size_t)y * out_w + x) * 3u;
                p[0] = (unsigned char)pix.data.range(7, 0);
                p[1] = (unsigned char)pix.data.range(15, 8);
                p[2] = (unsigned char)pix.data.range(23, 16);
            }
        }
    }

    size_t mism = 0;
    for (size_t i = 0; i < gold.size(); ++i)
        if (dout[i] != gold[i]) ++mism;

    printf("Mismatch bytes: %zu / %zu\n", mism, gold.size());
    if (mism) {
        fprintf(stderr, "ERROR: Test Failed.\n");
        return 1;
    }
    printf("Test Passed\n");
    return 0;
}

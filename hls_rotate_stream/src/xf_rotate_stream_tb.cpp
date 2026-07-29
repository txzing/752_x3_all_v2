/*
 * Multi-frame stream rotate TB (g++ / Vitis HLS csim).
 * Configures once, streams N frames, compares to golden.
 */

#include "xf_rotate_stream_accel.h"
#include "rotate_kernel.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

static bool load_bin(const std::string& path, std::vector<unsigned char>& buf, size_t expect) {
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) {
        fprintf(stderr, "ERROR: cannot open %s\n", path.c_str());
        return false;
    }
    buf.resize(expect);
    size_t n = fread(buf.data(), 1, expect, fp);
    fclose(fp);
    if (n != expect) {
        fprintf(stderr, "ERROR: short read %s (%zu / %zu)\n", path.c_str(), n, expect);
        return false;
    }
    return true;
}

static bool save_bin(const std::string& path, const std::vector<unsigned char>& buf) {
    FILE* fp = fopen(path.c_str(), "wb");
    if (!fp) {
        fprintf(stderr, "ERROR: cannot write %s\n", path.c_str());
        return false;
    }
    fwrite(buf.data(), 1, buf.size(), fp);
    fclose(fp);
    return true;
}

int main(int argc, char** argv) {
    /* args: <tb_data_dir> [direction] [num_frames] [height] [width] */
    std::string dir = (argc >= 2) ? argv[1] : "tb_data";
    int direction = (argc >= 3) ? atoi(argv[2]) : 0;
    int num_frames = (argc >= 4) ? atoi(argv[3]) : 3;
    int height = (argc >= 5) ? atoi(argv[4]) : 64;
    int width = (argc >= 6) ? atoi(argv[5]) : 128;

    if (direction < 0 || direction > 2) direction = 0;
    if (num_frames < 1) num_frames = 3;

    int out_h = 0, out_w = 0;
    rotate_get_out_size(height, width, direction, &out_h, &out_w);

    const size_t in_bytes = (size_t)height * (size_t)width * 3u;
    const size_t out_bytes = (size_t)out_h * (size_t)out_w * 3u;

    std::vector<unsigned char> din, dout, gold;
    if (!load_bin(dir + "/input_bgr.bin", din, in_bytes)) return 1;

    /* Build multi-frame input: frame0 = input, frame k = input XOR pattern */
    std::vector<unsigned char> frames_in((size_t)num_frames * in_bytes);
    for (int f = 0; f < num_frames; ++f) {
        for (size_t i = 0; i < in_bytes; ++i) {
            frames_in[(size_t)f * in_bytes + i] = (unsigned char)(din[i] ^ (unsigned char)(f * 17));
        }
    }

    /* Golden: rotate each frame independently (config-once semantics) */
    gold.assign((size_t)num_frames * out_bytes, 0);
    for (int f = 0; f < num_frames; ++f) {
        rotate_frame_rgb888(frames_in.data() + (size_t)f * in_bytes,
                            gold.data() + (size_t)f * out_bytes, height, width, direction);
    }

    dout.assign((size_t)num_frames * out_bytes, 0);

    printf("Stream C-sim: %d frames, in %dx%d dir=%d -> out %dx%d (config once)\n", num_frames, width,
           height, direction, out_w, out_h);

    if (rotate_stream_csim_run(frames_in.data(), num_frames, height, width, direction, dout.data()) !=
        0) {
        fprintf(stderr, "ERROR: rotate_stream_csim_run failed\n");
        return 1;
    }

    save_bin(dir + "/hls_stream_out_bgr.bin", dout);

    size_t mism = 0;
    size_t first = (size_t)-1;
    for (size_t i = 0; i < gold.size(); ++i) {
        if (dout[i] != gold[i]) {
            if (first == (size_t)-1) first = i;
            ++mism;
        }
    }

    double err_pct = 100.0 * (double)mism / (double)gold.size();
    printf("Mismatch bytes: %zu / %zu (%.6f%%)\n", mism, gold.size(), err_pct);
    if (mism) {
        printf("First mismatch @%zu hls=%u gold=%u\n", first, (unsigned)dout[first],
               (unsigned)gold[first]);
        fprintf(stderr, "ERROR: Test Failed.\n");
        return 1;
    }

    printf("Test Passed\n");
    return 0;
}

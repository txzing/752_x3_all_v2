#ifndef XF_ROTATE_STREAM_CONFIG_H
#define XF_ROTATE_STREAM_CONFIG_H

/*
 * Max synthesis bounds (board): 1920x3840 RGB888, 1 ppc.
 * C-sim / RTL TB use smaller runtime height/width (multiples of 32).
 */
#ifndef ROTATE_MAX_HEIGHT
#define ROTATE_MAX_HEIGHT 3840
#endif
#ifndef ROTATE_MAX_WIDTH
#define ROTATE_MAX_WIDTH 1920
#endif

#define ROTATE_BPP 3
#define ROTATE_AXIS_W 24
#define ROTATE_NUM_IN_BUF 2
#define ROTATE_NUM_OUT_BUF 2

/* One FB bytes at max resolution */
#define ROTATE_MAX_FRAME_BYTES \
    ((unsigned long)ROTATE_MAX_HEIGHT * (unsigned long)ROTATE_MAX_WIDTH * ROTATE_BPP)

#endif /* XF_ROTATE_STREAM_CONFIG_H */

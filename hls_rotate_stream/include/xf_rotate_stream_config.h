#ifndef XF_ROTATE_STREAM_CONFIG_H
#define XF_ROTATE_STREAM_CONFIG_H

/*
 * Max synthesis bounds (board): 1920x3840 RGB888.
 * NPC (PPC): 1 / 2 / 4 — AXIS_W = NPC * 24
 */
#ifndef ROTATE_MAX_HEIGHT
#define ROTATE_MAX_HEIGHT 3840
#endif
#ifndef ROTATE_MAX_WIDTH
#define ROTATE_MAX_WIDTH 1920
#endif

#ifndef ROTATE_NPC
#define ROTATE_NPC 1
#endif

#define ROTATE_BPP 3
#define ROTATE_AXIS_W (ROTATE_NPC * 24)
#define ROTATE_NUM_IN_BUF 2
#define ROTATE_NUM_OUT_BUF 2

#define ROTATE_MAX_FRAME_BYTES \
    ((unsigned long)ROTATE_MAX_HEIGHT * (unsigned long)ROTATE_MAX_WIDTH * ROTATE_BPP)

#endif /* XF_ROTATE_STREAM_CONFIG_H */

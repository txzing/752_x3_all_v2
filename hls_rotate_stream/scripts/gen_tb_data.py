#!/usr/bin/env python3
"""Generate deterministic RGB test frame for stream-rotate TB."""
import argparse
import pathlib
import struct


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--height", type=int, default=64)
    ap.add_argument("--width", type=int, default=128)
    ap.add_argument("--out-dir", type=pathlib.Path, default=pathlib.Path("tb_data"))
    args = ap.parse_args()

    args.out_dir.mkdir(parents=True, exist_ok=True)
    path = args.out_dir / "input_bgr.bin"
    with path.open("wb") as f:
        for y in range(args.height):
            for x in range(args.width):
                b = (x * 3 + y * 5) & 0xFF
                g = (x * 7 + y * 11) & 0xFF
                r = (x * 13 + y * 17) & 0xFF
                f.write(bytes((b, g, r)))
    print(f"Wrote {path} ({args.width}x{args.height})")


if __name__ == "__main__":
    main()

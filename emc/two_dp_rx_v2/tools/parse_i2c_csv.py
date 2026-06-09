#!/usr/bin/env python3
"""Parse logic analyzer I2C CSV for MAX96772 (addr 0x90/0x91)."""
import sys
from itertools import groupby

def parse_csv(path):
    rows = []
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for line in f:
            parts = line.strip().split(",", 2)
            if len(parts) < 3:
                continue
            rows.append(parts[2].strip())

    transactions = []

    def flush(txn):
        if txn and txn.get("type"):
            transactions.append(txn)

    i = 0
    while i < len(rows):
        ev = rows[i]
        if ev in ("Start", "Start repeat"):
            txn = {"type": None, "addr": None, "data": [], "start_ev": ev}
            i += 1
            while i < len(rows) and rows[i] != "Stop":
                e = rows[i]
                if e == "Start repeat":
                    flush(txn)
                    txn = {"type": None, "addr": None, "data": [], "start_ev": "Start repeat"}
                    i += 1
                    continue
                if e in ("Write", "Read"):
                    txn["type"] = e
                elif e.startswith("Address write:"):
                    txn["addr"] = int(e.split(":")[1].strip(), 16)
                elif e.startswith("Address read:"):
                    txn["addr"] = int(e.split(":")[1].strip(), 16)
                elif e.startswith("Data write:"):
                    txn["data"].append(int(e.split(":")[1].strip(), 16))
                elif e.startswith("Data read:"):
                    txn["data"].append(int(e.split(":")[1].strip(), 16))
                i += 1
            flush(txn)
            if i < len(rows) and rows[i] == "Stop":
                i += 1
        else:
            i += 1
    return transactions


def fmt_reg(h, lo):
    return f"0x{(h << 8) | lo:04X}"


def transactions_to_ops(transactions):
    ops = []
    idx = 0
    while idx < len(transactions):
        t = transactions[idx]
        if t["type"] == "Write" and len(t["data"]) >= 2:
            reg = fmt_reg(t["data"][0], t["data"][1])
            wr_vals = t["data"][2:]
            if (
                idx + 1 < len(transactions)
                and transactions[idx + 1]["type"] == "Read"
                and transactions[idx + 1]["start_ev"] == "Start repeat"
            ):
                rd = transactions[idx + 1]["data"]
                if wr_vals:
                    ops.append(("WRITE", reg, wr_vals))
                else:
                    ops.append(("READ", reg, rd))
                idx += 2
                continue
            if wr_vals:
                ops.append(("WRITE", reg, wr_vals))
            else:
                ops.append(("WRITE_PTR", reg, []))
            idx += 1
            continue

        if t["type"] == "Write":
            if len(t["data"]) >= 2:
                reg = fmt_reg(t["data"][0], t["data"][1])
                ops.append(("WRITE", reg, t["data"][2:]))
            else:
                ops.append(("WRITE_RAW", "?", t["data"]))
        elif t["type"] == "Read":
            ops.append(("READ_RAW", "?", t["data"]))
        idx += 1
    return ops


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else r"c:\Users\txzing\Desktop\decoder--260517-101949.csv"
    transactions = parse_csv(path)
    ops = transactions_to_ops(transactions)

    print(f"File: {path}")
    print(f"I2C segments: {len(transactions)}, parsed ops: {len(ops)}")
    print(f"7-bit slave: 0x{transactions[0]['addr'] >> 1:02X} (W=0x90, R=0x91)\n")

    def op_key(o):
        return (o[0], o[1], tuple(o[2]))

    unique = []
    for k, g in groupby(ops, key=op_key):
        unique.append((k, sum(1 for _ in g)))

    print("=== Unique register operations (by count) ===")
    for (kind, reg, vals), cnt in sorted(unique, key=lambda x: -x[1]):
        vstr = ", ".join(f"0x{b:02X}" for b in vals) if vals else "(none)"
        print(f"  {cnt:4d}x  {kind:6s}  {reg}  ->  {vstr}")

    # Init sequence: first contiguous block until repeat pattern stabilizes
    print("\n=== Full init sequence (first occurrence, deduped consecutive) ===")
    seen_block = []
    prev = None
    seq_num = 0
    for o in ops:
        key = (o[0], o[1], tuple(o[2]))
        if key == prev:
            continue
        prev = key
        seq_num += 1
        kind, reg, vals = o
        vstr = ", ".join(f"0x{b:02X}" for b in vals) if vals else ""
        if kind == "READ":
            print(f"  {seq_num:3d}. READ  {reg}  <=  {vstr}")
        else:
            print(f"  {seq_num:3d}. WRITE {reg}  =   {vstr}")

    # Export compact table for first 200 unique sequential ops
    print("\n=== First 200 sequential ops (no collapse) ===")
    for i, o in enumerate(ops[:200]):
        kind, reg, vals = o
        vstr = ", ".join(f"0x{b:02X}" for b in vals) if vals else ""
        if kind == "READ":
            print(f"  {i+1:3d}. READ  {reg}  <=  {vstr}")
        else:
            print(f"  {i+1:3d}. WRITE {reg}  =   {vstr}")


def summary_report(ops):
    from collections import Counter

    writes = [(r, tuple(v)) for k, r, v in ops if k == "WRITE"]
    reads = [(r, tuple(v)) for k, r, v in ops if k == "READ"]
    wc, rc = Counter(writes), Counter(reads)

    print("\n=== CONFIG WRITES (deduped, capture order) ===")
    seen = set()
    n = 0
    for k, r, v in ops:
        if k != "WRITE":
            continue
        t = (r, tuple(v))
        if t in seen:
            continue
        seen.add(t)
        n += 1
        vstr = ", ".join(f"0x{b:02X}" for b in v)
        print(f"  {n:2d}. WRITE {r} = {vstr}")

    print(f"\nTotals: {len(writes)} write ops ({len(wc)} unique), {len(reads)} read ops")
    print("Read polling:")
    for (r, v), c in rc.most_common():
        vstr = ", ".join(f"0x{b:02X}" for b in v)
        print(f"  READ {r} -> {vstr}  x{c}")

    for reg in ("0x0010", "0x0001", "0x0013", "0x0108", "0x01DC"):
        w = sum(1 for k, r, _ in ops if k == "WRITE" and r == reg)
        rd = sum(1 for k, r, _ in ops if k == "READ" and r == reg)
        if w or rd:
            print(f"  {reg}: {w} writes, {rd} reads")


if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else r"c:\Users\txzing\Desktop\decoder--260517-101949.csv"
    transactions = parse_csv(path)
    ops = transactions_to_ops(transactions)
    print(f"File: {path}")
    print(f"I2C segments: {len(transactions)}, parsed ops: {len(ops)}")
    if transactions:
        print(f"7-bit slave: 0x{transactions[0]['addr'] >> 1:02X} (W=0x90, R=0x91)\n")
    summary_report(ops)

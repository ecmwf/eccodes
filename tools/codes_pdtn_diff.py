#!/usr/bin/env python3

import argparse
import json
import os
import sys

from pdtn_matrix_common import ensure_start_pdtn, read_matrix, template_keyset


def parse_args():
    parser = argparse.ArgumentParser(
        description="Compare two PDTNs using keys_in_PDTNS and report key differences."
    )
    parser.add_argument("--from-pdtn", dest="from_pdtn", type=int, required=True)
    parser.add_argument("--to-pdtn", dest="to_pdtn", type=int, required=True)
    parser.add_argument("--matrix-csv", dest="matrix_csv", default="keys_in_PDTNS.csv")
    parser.add_argument("--matrix-pickle", dest="matrix_pickle", default="keys_in_PDTNS.pickle")
    parser.add_argument("--only", choices=["all", "added", "removed", "common"], default="all")
    parser.add_argument("--show", type=int, default=0, help="Limit printed keys per section (0 = all)")
    parser.add_argument("--json", action="store_true")
    return parser.parse_args()


def limit_items(items, show):
    if show <= 0 or len(items) <= show:
        return items, 0
    return items[:show], len(items) - show


def main():
    args = parse_args()
    base_dir = os.path.dirname(os.path.abspath(__file__))

    try:
        df, _, _ = read_matrix(args.matrix_csv, args.matrix_pickle, base_dir)
        ensure_start_pdtn(df, args.from_pdtn)
        ensure_start_pdtn(df, args.to_pdtn)
    except RuntimeError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    from_keys = template_keyset(df, args.from_pdtn)
    to_keys = template_keyset(df, args.to_pdtn)

    added = sorted(to_keys - from_keys)
    removed = sorted(from_keys - to_keys)
    common = sorted(from_keys & to_keys)

    report = {
        "from_pdtn": args.from_pdtn,
        "to_pdtn": args.to_pdtn,
        "counts": {
            "from": len(from_keys),
            "to": len(to_keys),
            "added": len(added),
            "removed": len(removed),
            "common": len(common),
        },
        "added": added,
        "removed": removed,
        "common": common,
    }

    if args.json:
        print(json.dumps(report, indent=2, sort_keys=False))
        return 0

    print(f"PDTN diff: {args.from_pdtn} -> {args.to_pdtn}")
    print(
        "Counts: "
        f"from={report['counts']['from']}, to={report['counts']['to']}, "
        f"added={report['counts']['added']}, removed={report['counts']['removed']}, "
        f"common={report['counts']['common']}"
    )

    sections = []
    if args.only in {"all", "added"}:
        sections.append(("Added", added))
    if args.only in {"all", "removed"}:
        sections.append(("Removed", removed))
    if args.only in {"all", "common"}:
        sections.append(("Common", common))

    for title, keys in sections:
        subset, remaining = limit_items(keys, args.show)
        print(f"\n{title} keys ({len(keys)}):")
        if not subset:
            print("  (none)")
            continue
        for key in subset:
            print(f"  - {key}")
        if remaining:
            print(f"  ... {remaining} more")

    return 0


if __name__ == "__main__":
    sys.exit(main())

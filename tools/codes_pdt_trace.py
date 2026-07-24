#!/usr/bin/env python3

import argparse
import json
import os
import sys

from pdtn_matrix_common import (
    choose_next_templates,
    ensure_start_pdtn,
    load_aliases,
    load_pdtn_names,
    parse_keys,
    rank_candidates,
    read_matrix,
    resolve_lookup_key,
    transition_metrics,
    validate_keys,
)


def parse_args():
    parser = argparse.ArgumentParser(
        description=(
            "Trace GRIB2 PDTN auto-selection decisions key-by-key, including "
            "candidate rankings and transition metrics."
        )
    )
    parser.add_argument("-s", "--startpdtn", dest="start_pdtn", type=int, required=True)
    parser.add_argument("-k", "--key", dest="keys", action="append", nargs="+", required=True)
    parser.add_argument("--matrix-csv", dest="matrix_csv", default="keys_in_PDTNS.csv")
    parser.add_argument("--matrix-pickle", dest="matrix_pickle", default="keys_in_PDTNS.pickle")
    parser.add_argument("--aliases-csv", dest="aliases_csv", default="keys_in_PDTNS_aliases.csv")
    parser.add_argument("--show-candidates", type=int, default=5, help="How many ranked candidates to show")
    parser.add_argument("--json", action="store_true", help="Emit machine-readable JSON")
    return parser.parse_args()


def run_trace(args):
    base_dir = os.path.dirname(os.path.abspath(__file__))
    df, matrix_csv, _ = read_matrix(args.matrix_csv, args.matrix_pickle, base_dir)
    aliases, aliases_csv = load_aliases(args.aliases_csv, base_dir)
    pdtn_names = load_pdtn_names(base_dir)
    keys = parse_keys(args.keys)

    ensure_start_pdtn(df, args.start_pdtn)
    validate_keys(df, keys, aliases)

    current_pdtn = args.start_pdtn
    events = []

    for requested_key in keys:
        resolved_key, alias_used = resolve_lookup_key(df, current_pdtn, requested_key, aliases)
        candidates, reason = choose_next_templates(df, current_pdtn, resolved_key)

        event = {
            "requested_key": requested_key,
            "resolved_key": resolved_key,
            "alias_used": bool(alias_used),
            "from_pdtn": int(current_pdtn),
            "reason": reason,
            "candidates": [int(x) for x in candidates],
            "tied_candidates": len(candidates) > 1,
            "ranked_candidates": rank_candidates(df, current_pdtn, resolved_key, limit=max(1, args.show_candidates)),
        }

        if reason == "not-found":
            event["to_pdtn"] = int(current_pdtn)
        elif reason == "already-present":
            event["to_pdtn"] = int(current_pdtn)
        else:
            target = int(candidates[0])
            event["to_pdtn"] = target
            event["metrics"] = transition_metrics(df, current_pdtn, target, resolved_key)
            current_pdtn = target

        events.append(event)

    report = {
        "start_pdtn": args.start_pdtn,
        "final_pdtn": int(current_pdtn),
        "matrix_csv": matrix_csv,
        "aliases_csv": aliases_csv,
        "events": events,
    }
    return report, pdtn_names


def print_human(report, pdtn_names):
    print(f"Start PDTN: {report['start_pdtn']}")
    for ev in report["events"]:
        alias_note = f" (alias->{ev['resolved_key']})" if ev["alias_used"] else ""
        print(
            f"\nKey: {ev['requested_key']}{alias_note}\n"
            f"  Reason: {ev['reason']}\n"
            f"  Transition: {ev['from_pdtn']} -> {ev['to_pdtn']}"
        )

        if ev["candidates"]:
            labels = []
            for pdtn in ev["candidates"]:
                name = pdtn_names.get(pdtn)
                labels.append(f"{pdtn} ({name})" if name else str(pdtn))
            print("  Winners: " + ", ".join(labels))

        metrics = ev.get("metrics")
        if metrics:
            print(
                "  Metrics: "
                f"overlap={metrics['overlap']}/{metrics['required']}, "
                f"missing={metrics['missing']}, extra={metrics['extra']}, "
                f"targetKeys={metrics['target_total']}, "
                f"overlapRatio={metrics['overlap_ratio']:.3f}"
            )

        print("  Ranked candidates:")
        for row in ev["ranked_candidates"]:
            name = pdtn_names.get(row["pdtn"], "")
            suffix = f" [{name}]" if name else ""
            print(
                f"    - {row['pdtn']}{suffix}: superset={row['superset']}, "
                f"overlap={row['overlap']}/{row['required']}, missing={row['missing']}, "
                f"extra={row['extra']}, total={row['total']}"
            )

    print(f"\nFinal PDTN: {report['final_pdtn']}")


def main():
    args = parse_args()
    try:
        report, names = run_trace(args)
    except RuntimeError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    if args.json:
        print(json.dumps(report, indent=2, sort_keys=False))
    else:
        print_human(report, names)

    return 0


if __name__ == "__main__":
    sys.exit(main())

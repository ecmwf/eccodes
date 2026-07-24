#!/usr/bin/env python3

import argparse
import json
import os
import sys

from pdtn_matrix_common import (
    choose_next_templates,
    ensure_start_pdtn,
    get_pdtn_from_grib,
    load_aliases,
    parse_keys,
    parse_set_expression,
    read_matrix,
    resolve_lookup_key,
    transition_metrics,
    validate_keys,
)


def parse_args():
    parser = argparse.ArgumentParser(
        description=(
            "Preflight a grib_set operation without writing output: parse keys, "
            "predict PDTN transitions, and report fallback risk."
        )
    )
    parser.add_argument("--input", dest="input_file", help="Input GRIB file used to detect start PDTN")
    parser.add_argument("--startpdtn", dest="start_pdtn", type=int, help="Override start PDTN")
    parser.add_argument("--set", dest="set_expr", help="grib_set style assignments: key1=v1,key2=v2")
    parser.add_argument("-k", "--key", dest="keys", action="append", nargs="+", help="Explicit keys")
    parser.add_argument("--grib-get-bin", default="grib_get", help="Path to grib_get binary")
    parser.add_argument("--matrix-csv", dest="matrix_csv", default="keys_in_PDTNS.csv")
    parser.add_argument("--matrix-pickle", dest="matrix_pickle", default="keys_in_PDTNS.pickle")
    parser.add_argument("--aliases-csv", dest="aliases_csv", default="keys_in_PDTNS_aliases.csv")
    parser.add_argument("--json", action="store_true")
    return parser.parse_args()


def derive_keys(args):
    keys = []
    if args.set_expr:
        keys.extend(parse_set_expression(args.set_expr))
    if args.keys:
        keys.extend(parse_keys(args.keys))
    if not keys:
        raise RuntimeError("Provide keys via --set and/or -k")
    return keys


def resolve_start_pdtn(args):
    if args.start_pdtn is not None:
        return args.start_pdtn
    if not args.input_file:
        raise RuntimeError("Provide --startpdtn or --input")
    return get_pdtn_from_grib(args.input_file, grib_get_bin=args.grib_get_bin)


def main():
    args = parse_args()
    base_dir = os.path.dirname(os.path.abspath(__file__))

    try:
        keys = derive_keys(args)
        start_pdtn = resolve_start_pdtn(args)
        df, matrix_csv, _ = read_matrix(args.matrix_csv, args.matrix_pickle, base_dir)
        aliases, aliases_csv = load_aliases(args.aliases_csv, base_dir)
        ensure_start_pdtn(df, start_pdtn)
        validate_keys(df, keys, aliases)
    except RuntimeError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    events = []
    current_pdtn = start_pdtn

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
            "to_pdtn": int(current_pdtn),
        }

        if reason not in {"already-present", "not-found"} and candidates:
            target = int(candidates[0])
            event["to_pdtn"] = target
            event["metrics"] = transition_metrics(df, current_pdtn, target, resolved_key)
            current_pdtn = target

        events.append(event)

    fallback_count = sum(1 for ev in events if ev["reason"] == "fallback-best-overlap")
    not_found_count = sum(1 for ev in events if ev["reason"] == "not-found")

    report = {
        "start_pdtn": int(start_pdtn),
        "final_pdtn": int(current_pdtn),
        "input_file": args.input_file,
        "set_expr": args.set_expr,
        "keys": keys,
        "matrix_csv": matrix_csv,
        "aliases_csv": aliases_csv,
        "summary": {
            "fallback_transitions": fallback_count,
            "not_found": not_found_count,
            "risk": "HIGH" if fallback_count or not_found_count else "LOW",
        },
        "events": events,
    }

    if args.json:
        print(json.dumps(report, indent=2, sort_keys=False))
        return 0

    print(f"Start PDTN: {report['start_pdtn']}")
    print(f"Keys: {', '.join(report['keys'])}")
    for ev in events:
        alias_note = f" (alias->{ev['resolved_key']})" if ev["alias_used"] else ""
        line = (
            f"- {ev['requested_key']}: {ev['from_pdtn']} -> {ev['to_pdtn']} "
            f"({ev['reason']}){alias_note}"
        )
        metrics = ev.get("metrics")
        if metrics:
            line += (
                "; "
                f"overlap={metrics['overlap']}/{metrics['required']}, "
                f"missing={metrics['missing']}, extra={metrics['extra']}"
            )
        print(line)

    print(f"Final PDTN: {report['final_pdtn']}")
    print(
        "Risk summary: "
        f"{report['summary']['risk']} "
        f"(fallback={report['summary']['fallback_transitions']}, "
        f"not_found={report['summary']['not_found']})"
    )

    if report["summary"]["risk"] == "HIGH":
        print("Recommendation: review with codes_pdt_trace.py and enforce policy with codes_pdt_guard.py")

    return 0


if __name__ == "__main__":
    sys.exit(main())

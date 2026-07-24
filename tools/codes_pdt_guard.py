#!/usr/bin/env python3

import argparse
import csv
import glob
import json
import os
import sys

import pandas as pd


def defs_grib2_dir():
    return os.path.normpath(os.path.join(os.path.dirname(__file__), "..", "definitions", "grib2"))


def parse_args():
    parser = argparse.ArgumentParser(
        description=(
            "Preflight guard for GRIB2 PDTN auto-selection. "
            "Checks a requested key sequence and fails if configured risk limits are exceeded."
        )
    )
    parser.add_argument(
        "-s", "--startpdtn", dest="start_pdtn", type=int, required=True,
        help="Start PDTN of the message."
    )
    parser.add_argument(
        "-k", "--key", dest="keys", action="append", nargs="+", required=True,
        help="Key to set. Can be given as a list, repeated, and/or comma-separated."
    )
    parser.add_argument(
        "--matrix-csv", dest="matrix_csv", default="keys_in_PDTNS.csv",
        help="CSV matrix with key presence by template (default: keys_in_PDTNS.csv)."
    )
    parser.add_argument(
        "--matrix-pickle", dest="matrix_pickle", default="keys_in_PDTNS.pickle",
        help=(
            "Optional pickle file with PDTN index (default: keys_in_PDTNS.pickle). "
            "If present and shape-compatible, its index is used as PDTN list."
        )
    )
    parser.add_argument(
        "--aliases-csv", dest="aliases_csv", default="keys_in_PDTNS_aliases.csv",
        help=(
            "Optional alias mapping CSV with columns alias,canonical "
            "(default: keys_in_PDTNS_aliases.csv)."
        )
    )
    parser.add_argument(
        "--max-missing", dest="max_missing", type=int, default=0,
        help="Maximum allowed dropped required keys per transition (default: 0)."
    )
    parser.add_argument(
        "--min-overlap", dest="min_overlap", type=float, default=0.0,
        help="Minimum allowed overlap ratio for fallback transitions (0..1, default: 0.0)."
    )
    parser.add_argument(
        "--fail-on-fallback", action="store_true",
        help="Fail whenever fallback-best-overlap is used."
    )
    parser.add_argument(
        "--fail-on-tie", action="store_true",
        help="Fail if template selection has multiple equally good candidates."
    )
    parser.add_argument(
        "--json", action="store_true",
        help="Print machine-readable JSON report."
    )
    return parser.parse_args()


def normalise_presence_matrix(df):
    return df.notna() & (df != 0)


def read_matrix(matrix_csv, matrix_pickle):
    if not os.path.isabs(matrix_csv) and not os.path.exists(matrix_csv):
        matrix_csv = os.path.join(defs_grib2_dir(), matrix_csv)
    if not os.path.isabs(matrix_pickle) and not os.path.exists(matrix_pickle):
        matrix_pickle = os.path.join(defs_grib2_dir(), matrix_pickle)

    if not os.path.exists(matrix_csv):
        sys.exit(f"Matrix CSV not found: {matrix_csv}")

    df_csv = pd.read_csv(matrix_csv)

    first_col = str(df_csv.columns[0]) if len(df_csv.columns) > 0 else ""
    if len(df_csv.columns) > 0 and (
        first_col.startswith("Unnamed:") or
        first_col in {"index", "pdtn", "productDefinitionTemplateNumber"}
    ):
        candidate_index = df_csv.iloc[:, 0]
        if pd.api.types.is_integer_dtype(candidate_index):
            df_csv = df_csv.set_index(df_csv.columns[0])

    df = normalise_presence_matrix(df_csv)

    if matrix_pickle and os.path.exists(matrix_pickle):
        df_pickle = pd.read_pickle(matrix_pickle)
        if df_pickle.shape[0] == df.shape[0]:
            df.index = df_pickle.index

    try:
        df.index = df.index.astype(int)
    except Exception:
        pass

    return df


def parse_keys(raw_keys):
    out = []
    for group in raw_keys:
        for item in group:
            parts = [x.strip() for x in item.split(",") if x.strip()]
            out.extend(parts)
    return out


def load_aliases(aliases_csv):
    if aliases_csv and not os.path.isabs(aliases_csv) and not os.path.exists(aliases_csv):
        aliases_csv = os.path.join(defs_grib2_dir(), aliases_csv)

    aliases = {}
    if not aliases_csv or not os.path.exists(aliases_csv):
        return aliases

    try:
        with open(aliases_csv, newline="", encoding="utf-8") as handle:
            reader = csv.reader(handle)
            for row in reader:
                if not row:
                    continue
                first = row[0].strip() if len(row) > 0 else ""
                if not first or first.startswith("#"):
                    continue
                if len(row) < 2:
                    continue
                alias = row[0].strip()
                canonical = row[1].strip()
                if alias == "alias" and canonical == "canonical":
                    continue
                if alias and canonical:
                    aliases[alias] = canonical
    except OSError:
        return {}

    return aliases


def template_keyset(df, pdtn):
    return set(df.columns[df.loc[pdtn]])


def load_pdtn_names():
    names = {}
    base_dir = os.path.join(defs_grib2_dir(), "tables")
    table_paths = sorted(glob.glob(os.path.join(base_dir, "*", "4.0.table")))

    for path in table_paths:
        try:
            with open(path, encoding="utf-8") as handle:
                for line in handle:
                    line = line.strip()
                    if not line or line.startswith("#"):
                        continue
                    parts = line.split(None, 2)
                    if len(parts) < 3:
                        continue
                    try:
                        pdtn = int(parts[0])
                    except ValueError:
                        continue
                    names[pdtn] = parts[2]
        except OSError:
            continue

    return names


def resolve_lookup_key(df, current_pdtn, requested_key, aliases):
    current_keys = template_keyset(df, current_pdtn)

    if requested_key in current_keys:
        return requested_key, None

    if requested_key in df.columns:
        return requested_key, None

    canonical = aliases.get(requested_key)
    if canonical and canonical in df.columns:
        return canonical, canonical

    return requested_key, None


def choose_next_templates(df, current_pdtn, new_key):
    templates = list(df.index)
    current_keys = template_keyset(df, current_pdtn)

    if new_key in current_keys:
        return [current_pdtn], "already-present"

    required = set(current_keys)
    required.add(new_key)

    superset_candidates = []
    for pdtn in templates:
        keyset = template_keyset(df, pdtn)
        if required.issubset(keyset):
            additional = len(keyset - required)
            superset_candidates.append((additional, len(keyset), int(pdtn), pdtn))

    if superset_candidates:
        superset_candidates.sort()
        best_additional, best_len, _, _ = superset_candidates[0]
        winners = [
            candidate_pdtn
            for additional, total_len, _, candidate_pdtn in superset_candidates
            if additional == best_additional and total_len == best_len
        ]
        return winners, "superset-min-extra"

    fallback_candidates = []
    for pdtn in templates:
        keyset = template_keyset(df, pdtn)
        if new_key in keyset:
            overlap = len(required & keyset)
            additional = len(keyset - required)
            fallback_candidates.append((-overlap, additional, len(keyset), int(pdtn), pdtn))

    if not fallback_candidates:
        return [], "not-found"

    fallback_candidates.sort()
    best_overlap, best_additional, best_len, _, _ = fallback_candidates[0]
    winners = [
        candidate_pdtn
        for overlap, additional, total_len, _, candidate_pdtn in fallback_candidates
        if overlap == best_overlap and additional == best_additional and total_len == best_len
    ]
    return winners, "fallback-best-overlap"


def transition_metrics(df, current_pdtn, target_pdtn, new_key):
    current_keys = template_keyset(df, current_pdtn)
    target_keys = template_keyset(df, target_pdtn)
    required = set(current_keys)
    required.add(new_key)

    overlap = len(required & target_keys)
    missing = len(required - target_keys)
    extra = len(target_keys - required)
    required_count = len(required)
    overlap_ratio = float(overlap) / float(required_count) if required_count else 1.0

    return {
        "required": required_count,
        "overlap": overlap,
        "missing": missing,
        "extra": extra,
        "target_total": len(target_keys),
        "overlap_ratio": overlap_ratio,
    }


def evaluate_guard(events, args):
    issues = []

    for ev in events:
        if ev["reason"] == "not-found":
            issues.append(f"Key '{ev['requested_key']}' cannot be routed by matrix from PDTN {ev['from_pdtn']}")
            continue

        if ev["tied_candidates"] and args.fail_on_tie:
            issues.append(
                f"Key '{ev['requested_key']}' has tied candidates: {','.join(str(x) for x in ev['candidates'])}"
            )

        if ev["reason"] == "fallback-best-overlap" and args.fail_on_fallback:
            issues.append(
                f"Key '{ev['requested_key']}' required fallback (PDTN {ev['from_pdtn']} -> {ev['to_pdtn']})"
            )

        metrics = ev.get("metrics")
        if not metrics:
            continue

        if metrics["missing"] > args.max_missing:
            issues.append(
                f"Key '{ev['requested_key']}' drops {metrics['missing']} required keys "
                f"(limit={args.max_missing})"
            )

        if ev["reason"] == "fallback-best-overlap" and metrics["overlap_ratio"] < args.min_overlap:
            issues.append(
                f"Key '{ev['requested_key']}' overlap ratio {metrics['overlap_ratio']:.3f} "
                f"is below limit {args.min_overlap:.3f}"
            )

    return issues


def print_human_report(start_pdtn, final_pdtn, events, issues, pdtn_names):
    print(f"Start PDTN: {start_pdtn}")
    for ev in events:
        key = ev["requested_key"]
        alias_note = f" (alias->{ev['resolved_key']})" if ev["alias_used"] else ""

        if ev["reason"] == "not-found":
            print(f"- {key}: NOT FOUND in matrix{alias_note}")
            continue

        if ev["reason"] == "already-present":
            print(f"- {key}: already present in PDTN {ev['from_pdtn']}{alias_note}")
            continue

        to_name = pdtn_names.get(ev["to_pdtn"], "")
        to_label = f" [{to_name}]" if to_name else ""
        tie_note = " (tied candidates)" if ev["tied_candidates"] else ""

        m = ev["metrics"]
        print(
            f"- {key}: {ev['from_pdtn']} -> {ev['to_pdtn']} ({ev['reason']}){alias_note}{tie_note}; "
            f"overlap={m['overlap']}/{m['required']}, missing={m['missing']}, extra={m['extra']}, "
            f"targetKeys={m['target_total']}{to_label}"
        )

    print(f"Final PDTN: {final_pdtn}")

    if issues:
        print("GUARD RESULT: FAIL")
        for issue in issues:
            print(f"  * {issue}")
    else:
        print("GUARD RESULT: PASS")


def build_report(start_pdtn, final_pdtn, events, issues):
    return {
        "start_pdtn": start_pdtn,
        "final_pdtn": final_pdtn,
        "guard_result": "FAIL" if issues else "PASS",
        "issues": issues,
        "events": events,
    }


def main():
    args = parse_args()

    if args.max_missing < 0:
        sys.exit("--max-missing must be >= 0")
    if not (0.0 <= args.min_overlap <= 1.0):
        sys.exit("--min-overlap must be in [0,1]")

    df = read_matrix(args.matrix_csv, args.matrix_pickle)
    keys = parse_keys(args.keys)
    aliases = load_aliases(args.aliases_csv)
    pdtn_names = load_pdtn_names()

    if args.start_pdtn not in df.index:
        available = ",".join(str(x) for x in list(df.index)[:20])
        sys.exit(
            f"productDefinitionTemplateNumber {args.start_pdtn} is not in matrix index. "
            f"First templates: {available}"
        )

    unknown = [k for k in keys if k not in df.columns and aliases.get(k) not in df.columns]
    if unknown:
        sys.exit("Key(s) not defined in matrix: " + ", ".join(unknown))

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
            "to_pdtn": int(current_pdtn) if reason in {"already-present", "not-found"} else int(candidates[0]),
        }

        if reason not in {"already-present", "not-found"} and candidates:
            event["metrics"] = transition_metrics(df, current_pdtn, candidates[0], resolved_key)
            current_pdtn = candidates[0]

        events.append(event)

    final_pdtn = int(current_pdtn)
    issues = evaluate_guard(events, args)

    if args.json:
        report = build_report(args.start_pdtn, final_pdtn, events, issues)
        print(json.dumps(report, indent=2, sort_keys=False))
    else:
        print_human_report(args.start_pdtn, final_pdtn, events, issues, pdtn_names)

    return 1 if issues else 0


if __name__ == "__main__":
    sys.exit(main())

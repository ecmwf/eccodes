#!/usr/bin/env python3

import argparse
import glob
import os
import sys

import pandas as pd


def parse_args():
    parser = argparse.ArgumentParser(
        description=(
            "Sequentially checks whether each key is available in the current "
            "GRIB2 productDefinitionTemplateNumber (PDTN). If not, selects the "
            "closest matching PDTN using the keys_in_PDTNS matrix."
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
    return parser.parse_args()


def normalise_presence_matrix(df):
    """Convert matrix values to booleans: True if key is defined in template."""
    return df.notna() & (df != 0)


def read_matrix(matrix_csv, matrix_pickle):
    if not os.path.exists(matrix_csv):
        sys.exit(f"Matrix CSV not found: {matrix_csv}")

    df_csv = pd.read_csv(matrix_csv)

    # Handle CSVs that include an explicit PDTN/index column.
    first_col = str(df_csv.columns[0]) if len(df_csv.columns) > 0 else ""
    if len(df_csv.columns) > 0 and (
        first_col.startswith("Unnamed:") or
        first_col in {"index", "pdtn", "productDefinitionTemplateNumber"}
    ):
        candidate_index = df_csv.iloc[:, 0]
        if pd.api.types.is_integer_dtype(candidate_index):
            df_csv = df_csv.set_index(df_csv.columns[0])

    df = normalise_presence_matrix(df_csv)

    # If available, use pickle index as authoritative PDTN IDs.
    if matrix_pickle and os.path.exists(matrix_pickle):
        df_pickle = pd.read_pickle(matrix_pickle)
        if df_pickle.shape[0] == df.shape[0]:
            df.index = df_pickle.index

    # Ensure template IDs are ints where possible.
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


def template_keyset(df, pdtn):
    return set(df.columns[df.loc[pdtn]])


def load_pdtn_names():
    names = {}
    base_dir = os.path.join(os.path.dirname(__file__), "eccodes", "definitions", "grib2", "tables")
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


def choose_next_templates(df, current_pdtn, new_key):
    """
    Select next PDTN using matrix-driven matching.
    Priority:
    1) Superset match of (current template keys + new key), minimal additional keys.
    2) Fallback to templates containing new key with maximal overlap and minimal extras.
    """
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


def main():
    args = parse_args()
    df = read_matrix(args.matrix_csv, args.matrix_pickle)
    keys = parse_keys(args.keys)
    pdtn_names = load_pdtn_names()

    if args.start_pdtn not in df.index:
        available = ",".join(str(x) for x in list(df.index)[:20])
        sys.exit(
            f"ProductDefinitionTemplateNumber {args.start_pdtn} is not in matrix index. "
            f"First templates: {available}"
        )

    unknown_keys = [k for k in keys if k not in df.columns]
    if unknown_keys:
        sys.exit("Key(s) not defined in matrix: " + ", ".join(unknown_keys))

    current_pdtn = args.start_pdtn
    print(f"Start PDTN: {current_pdtn}")

    for key in keys:
        next_pdtns, reason = choose_next_templates(df, current_pdtn, key)
        if not next_pdtns:
            print(f"{key}: no template contains this key. Keep PDTN {current_pdtn}")
            continue

        if next_pdtns == [current_pdtn]:
            print(f"{key}: already available in PDTN {current_pdtn}")
        else:
            print(f"{key}: PDTN {current_pdtn} -> {','.join(str(p) for p in next_pdtns)} ({reason})")
            for pdtn in next_pdtns:
                label = pdtn_names.get(pdtn, "")
                if label:
                    print(f"{pdtn}\t{label}")
                else:
                    print(pdtn)
            current_pdtn = next_pdtns[0]

    print(f"Final PDTN: {current_pdtn}")


if __name__ == "__main__":
    main()

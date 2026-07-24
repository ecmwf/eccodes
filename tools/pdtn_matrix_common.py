#!/usr/bin/env python3

import csv
import glob
import os
import subprocess

import pandas as pd


def get_grib2_defs_dir(base_dir):
    return os.path.normpath(os.path.join(base_dir, "..", "definitions", "grib2"))


def resolve_data_path(path_value, default_name, base_dir):
    defs_dir = get_grib2_defs_dir(base_dir)
    if path_value:
        if os.path.isabs(path_value):
            return path_value
        if os.path.exists(path_value):
            return path_value
        candidate = os.path.join(base_dir, path_value)
        if os.path.exists(candidate):
            return candidate
        return os.path.join(defs_dir, path_value)
    return os.path.join(defs_dir, default_name)


def normalise_presence_matrix(df):
    return df.notna() & (df != 0)


def read_matrix(matrix_csv, matrix_pickle, base_dir):
    matrix_csv = resolve_data_path(matrix_csv, "keys_in_PDTNS.csv", base_dir)
    matrix_pickle = resolve_data_path(matrix_pickle, "keys_in_PDTNS.pickle", base_dir)

    if not os.path.exists(matrix_csv):
        raise RuntimeError(f"Matrix CSV not found: {matrix_csv}")

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

    return df, matrix_csv, matrix_pickle


def parse_keys(raw_keys):
    out = []
    for group in raw_keys:
        for item in group:
            parts = [x.strip() for x in item.split(",") if x.strip()]
            out.extend(parts)
    return out


def parse_set_expression(set_expr):
    keys = []
    if not set_expr.strip():
        return keys

    for part in set_expr.split(","):
        item = part.strip()
        if not item:
            continue
        if "=" not in item:
            raise RuntimeError(f"Invalid assignment '{item}'. Expected key=value.")
        key, _ = item.split("=", 1)
        key = key.strip()
        if not key:
            raise RuntimeError(f"Invalid assignment '{item}'. Empty key.")
        keys.append(key)

    return keys


def load_aliases(aliases_csv, base_dir):
    aliases = {}
    aliases_csv = resolve_data_path(aliases_csv, "keys_in_PDTNS_aliases.csv", base_dir)
    if not aliases_csv or not os.path.exists(aliases_csv):
        return aliases, aliases_csv

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
        return {}, aliases_csv

    return aliases, aliases_csv


def template_keyset(df, pdtn):
    return set(df.columns[df.loc[pdtn]])


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


def rank_candidates(df, current_pdtn, new_key, limit=5):
    current_keys = template_keyset(df, current_pdtn)
    required = set(current_keys)
    required.add(new_key)

    superset = []
    fallback = []
    for pdtn in list(df.index):
        keyset = template_keyset(df, pdtn)
        if new_key not in keyset:
            continue

        overlap = len(required & keyset)
        additional = len(keyset - required)
        total = len(keyset)

        row = {
            "pdtn": int(pdtn),
            "overlap": overlap,
            "required": len(required),
            "missing": len(required - keyset),
            "extra": additional,
            "total": total,
            "superset": required.issubset(keyset),
        }

        if row["superset"]:
            superset.append((additional, total, int(pdtn), row))
        else:
            fallback.append((-overlap, additional, total, int(pdtn), row))

    superset.sort()
    fallback.sort()

    ranked = [x[3] for x in superset[:limit]]
    remaining = max(0, limit - len(ranked))
    ranked.extend([x[4] for x in fallback[:remaining]])
    return ranked


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


def load_pdtn_names(base_dir):
    names = {}
    table_dir = os.path.join(get_grib2_defs_dir(base_dir), "tables")
    table_paths = sorted(glob.glob(os.path.join(table_dir, "*", "4.0.table")))

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


def ensure_start_pdtn(df, start_pdtn):
    if start_pdtn in df.index:
        return
    available = ",".join(str(x) for x in list(df.index)[:20])
    raise RuntimeError(
        f"productDefinitionTemplateNumber {start_pdtn} is not in matrix index. "
        f"First templates: {available}"
    )


def validate_keys(df, keys, aliases):
    unknown = [k for k in keys if k not in df.columns and aliases.get(k) not in df.columns]
    if unknown:
        raise RuntimeError("Key(s) not defined in matrix: " + ", ".join(unknown))


def get_pdtn_from_grib(grib_file, grib_get_bin="grib_get"):
    cmd = [grib_get_bin, "-p", "productDefinitionTemplateNumber", grib_file]
    proc = subprocess.run(cmd, check=False, capture_output=True, text=True)
    if proc.returncode != 0:
        stderr = proc.stderr.strip() or proc.stdout.strip()
        raise RuntimeError(f"Failed to read PDTN from '{grib_file}' using {grib_get_bin}: {stderr}")

    value = proc.stdout.strip().splitlines()
    if not value:
        raise RuntimeError(f"No output from {grib_get_bin} for file '{grib_file}'")

    token = value[0].strip().split()[0]
    try:
        return int(token)
    except ValueError as exc:
        raise RuntimeError(f"Unexpected PDTN output '{value[0]}' from {grib_get_bin}") from exc

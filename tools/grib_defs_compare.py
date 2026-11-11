#!/usr/bin/env python3
"""Compare the parameters (or chemicals) of two ecCodes definitions sets.

Based on grib_defs, the ecCodes tool listing the parameters and chemicals of
the concept files (subcommands params and chems). This script does not read
any definitions itself: it runs grib_defs once per side with --show-encoding
--format json and compares the two outputs, so it finds exactly the entries
ecCodes itself finds (same parser, same definitions lookup, MEMFS included).
A grib_defs executable is required (--grib-defs, $GRIB_DEFS or PATH).

Two modes, which cannot be mixed:

  Definitions folders: --definition-old and --definition-new. One grib_defs
  (--grib-defs, $GRIB_DEFS or PATH) reads each folder with --definitions-root,
  so only that folder is used (no built-in definitions, no
  ECCODES_EXTRA_DEFINITION_PATH).

  Binaries: --grib-defs-old and --grib-defs-new, e.g. of two ecCodes
  installations. Each binary uses its own definitions (its built-in path,
  e.g. MEMFS). ECCODES_DEFINITION_PATH and ECCODES_EXTRA_DEFINITION_PATH are
  removed from their environment, so they cannot replace both sides by the
  same set.


Examples:
  grib_defs_compare.py --definition-old /path/to/old/definitions \\
                       --definition-new /path/to/new/definitions
  grib_defs_compare.py --definition-old OLD --definition-new NEW \\
                       --new --changed-unit --format markdown
  grib_defs_compare.py --definition-old OLD --definition-new NEW \\
                       --changed-encoding --edition 2 --is_mtg2_switch 1
  grib_defs_compare.py --grib-defs-old /path/to/2.51/bin \\
                       --grib-defs-new /path/to/2.52/bin

Arguments after "--" are passed to both grib_defs runs unchanged, e.g.
-- --paramId '13*' or -- --name-regex 'anomaly', to restrict the comparison.

Exit status: 0 no differences, 1 differences found, 2 error.
Entries listed with --common do not count as differences.
"""

import argparse
import fnmatch
import json
import os
import shutil
import subprocess
import sys

# Fields per grib_defs subcommand: id, short name, long name, extra field
# compared by --changed-unit (params) or --changed-formula (chems).
KINDS = {
    "params": {"id": "paramId", "short": "shortName", "name": "name", "extra": "units"},
    "chems": {"id": "chemId", "short": "chemShortName", "name": "chemName", "extra": "chemFormula"},
}

# Change types in output order.
CHANGES = ["new-scope", "removed-scope", "moved", "new", "removed", "added-entry", "removed-entry", "changed-encoding", "renamed-shortName", "renamed-name",
           "changed-unit", "changed-formula", "common"]
# Not part of the default selection: they are not differences
NOT_DIFFERENCES = {"common"}


def scope_filter(rows, include, exclude):
    """Keep rows whose scope matches one of the include patterns and none of the exclude patterns."""
    def matches(scope, patterns):
        return any(fnmatch.fnmatchcase(scope or "", pat) for pat in patterns)
    return [r for r in rows
            if (not include or matches(r.get("scope"), include)) and not matches(r.get("scope"), exclude)]


def patterns(text):
    return [x.strip().rstrip("/") for x in (text or "").split(",") if x.strip()]


def run_grib_defs(grib_defs, kind, root, extra_args):
    """Start grib_defs for one definitions root (None: the binary's own definitions);
    returns the Popen object and the command."""
    cmd = [grib_defs, kind]
    env = None
    if root:
        cmd += ["--definitions-root", root]
    else:
        env = {k: v for k, v in os.environ.items()
               if k not in ("ECCODES_DEFINITION_PATH", "ECCODES_EXTRA_DEFINITION_PATH")}
    cmd += ["--show-encoding", "--format", "json"] + extra_args
    try:
        return subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, env=env), cmd
    except OSError as e:
        sys.stderr.write("Cannot run %s: %s\n" % (grib_defs, e.strerror))
        sys.exit(2)


def collect(proc, cmd):
    out, err = proc.communicate()
    # grib_defs: 0 results, 1 no results (prints []), 2 error
    if proc.returncode not in (0, 1):
        sys.stderr.write("Error running: %s\n%s" % (" ".join(cmd), err))
        sys.exit(2)
    try:
        return json.loads(out) if out.strip() else []
    except json.JSONDecodeError as e:
        sys.stderr.write("Cannot parse grib_defs JSON output of: %s\n%s\n" % (" ".join(cmd), e))
        sys.exit(2)


# Set by --ignore-whitespace: strip and collapse whitespace in names, units and formulas
IGNORE_WHITESPACE = False


def text_value(v):
    v = "" if v is None else str(v)
    return " ".join(v.split()) if IGNORE_WHITESPACE else v


def as_set(value):
    """grib_defs prints merged duplicates as arrays; normalise to a set of strings."""
    if value is None:
        return set()
    if isinstance(value, list):
        return {text_value(v) for v in value}
    return {text_value(value)}


def placement(sw, legacy):
    """Where an entry is used: GRIB1 (no switch), a legacy file, or an MTG2 switch."""
    if legacy:
        return "legacy"
    return "no switch" if sw is None else "switch %s" % sw


def encoding_text(enc):
    """Canonical text of one encoding, keys sorted, e.g. discipline=0, parameterCategory=0."""
    parts = []
    for k in sorted(enc):
        v = enc[k]
        if isinstance(v, list):
            v = "[" + ",".join("missing()" if x is None else str(x) for x in v) + "]"
        elif v is None:
            v = "missing()"
        parts.append("%s=%s" % (k, v))
    return ", ".join(parts)


def index_rows(rows, kind, match):
    """Group rows by entry key. Each key collects the sets of names and encodings.

    When one encoding maps to several ids in a concept file, grib_defs prints
    the ids (and the names) of that row as arrays. The names of such a row
    cannot be assigned to the ids with certainty, because every concept file
    is read on its own and equal values are merged. Names are therefore taken
    from rows with a single id; only for an id that appears in no such row
    the arrays are paired by position (equal lengths) or used as a whole.
    """
    f = KINDS[kind]
    entries = {}
    fields = ("short", "name", "extra")
    for r in rows:
        ids = r[f["id"]]
        ids = ids if isinstance(ids, list) else [ids]
        enc = encoding_text(r.get("encoding") or {})
        if match == "id":
            # Across scopes the same encoding may occur in several scopes, switches and legacy files;
            # keep scope and placement in the text
            enc = "%s [%s]: %s" % (r.get("scope"), placement(r.get("is_mtg2_switch"), r.get("is_legacy")), enc)
        for pos, ident in enumerate(ids):
            if match == "scope":
                key = (r.get("edition"), r.get("is_mtg2_switch"), r.get("is_legacy"), r.get("scope"), ident)
            elif match == "switch":
                # The selected scopes are taken as one, e.g. grib2 and grib2/localConcepts/ecmf
                key = (r.get("edition"), r.get("is_mtg2_switch"), r.get("is_legacy"), None, ident)
            else:
                key = (None, None, None, None, ident)
            e = entries.setdefault(key, {"enc": set(), "single": {k: set() for k in fields},
                                         "multi": {k: set() for k in fields}})
            e["enc"].add(enc)
            for field in fields:
                v = r.get(f[field])
                if len(ids) == 1:
                    e["single"][field] |= as_set(v)
                elif isinstance(v, list) and len(v) == len(ids):
                    e["multi"][field] |= as_set(v[pos])
                else:
                    e["multi"][field] |= as_set(v)
    for e in entries.values():
        for field in fields:
            e[field] = e["single"][field] or e["multi"][field]
        del e["single"], e["multi"]
    return entries


def shown(v):
    """Quote a value whose whitespace would otherwise be invisible, e.g. a trailing space."""
    return '"%s"' % v if v != v.strip() or "  " in v else v


def joined(values):
    return " / ".join(shown(v) for v in sorted(values))


def compare(old, new, wanted, kind, old_rows, new_rows):
    """Return the list of differences as dicts."""
    extra_change = "changed-unit" if kind == "params" else "changed-formula"
    diffs = []

    # Scopes (grib1, grib2, gribN/localConcepts/<centre>) present in only one set
    def scope_counts(rows):
        counts = {}
        for r in rows:
            n = len(r[KINDS[kind]["id"]]) if isinstance(r[KINDS[kind]["id"]], list) else 1
            counts[r.get("scope")] = counts.get(r.get("scope"), 0) + n
        return counts

    old_scopes, new_scopes = scope_counts(old_rows), scope_counts(new_rows)
    for scope in sorted(set(old_scopes) | set(new_scopes)):
        for change, present, absent in (("new-scope", new_scopes, old_scopes), ("removed-scope", old_scopes, new_scopes)):
            if change in wanted and scope in present and scope not in absent:
                text = "%s (%d entries)" % (scope, present[scope])
                diffs.append({"change": change, "edition": None, "is_mtg2_switch": None, "is_legacy": None,
                              "scope": scope, "id": None, "short": None, "name": None, "extra": None, "encoding": None,
                              "old": text if change == "removed-scope" else "",
                              "new": text if change == "new-scope" else ""})

    def add(change, key, short, old_val, new_val):
        if change in wanted:
            edition, sw, legacy, scope, ident = key
            # Names of the entry: from the new definitions, or the old ones for a removed entry
            e = new.get(key) or old.get(key)
            diffs.append({"change": change, "edition": edition, "is_mtg2_switch": sw, "is_legacy": legacy,
                          "scope": scope, "id": ident, "short": short, "name": joined(e["name"]),
                          "extra": joined(e["extra"]), "encoding": sorted(e["enc"]),
                          "old": old_val, "new": new_val})

    # Entries moved between MTG2 switches and/or legacy files: present only in the old definitions
    # under some placements and only in the new ones under others, with the same encodings.
    # Such groups are reported once as "moved" instead of as removed and new entries; name and
    # unit changes made at the same time are reported in addition.
    def signature(key, e):
        edition, sw, legacy, scope, ident = key
        return (edition, ident, frozenset(e["enc"]))

    def where(k, with_scope):
        """Placement of an entry, with its scope when scopes are compared separately."""
        text = placement(k[1], k[2])
        return "%s [%s]" % (k[3], text) if with_scope and k[3] is not None else text

    only_old = {k: v for k, v in old.items() if k not in new}
    only_new = {k: v for k, v in new.items() if k not in old}
    groups = {}
    for side, entries in (("old", only_old), ("new", only_new)):
        for k, e in entries.items():
            groups.setdefault(signature(k, e), {"old": [], "new": []})[side].append(k)
    moved_keys = set()
    moved_renames = []  # (old key, new key) of moved groups, checked for name changes below
    for sig, g in groups.items():
        if g["old"] and g["new"] and sig[0] is not None:
            moved_keys.update(g["old"] + g["new"])
            first = sorted(g["new"], key=sort_key)[0]
            if "moved" in wanted:
                edition, _, _, scope, ident = first
                e = new[first]
                # Moved between scopes, e.g. from grib2/localConcepts/ecmf to grib2: show the scopes
                cross_scope = len({k[3] for k in g["old"] + g["new"]}) > 1
                diffs.append({"change": "moved", "edition": edition, "is_mtg2_switch": None, "is_legacy": None,
                              "scope": None if cross_scope else scope, "id": ident, "short": joined(e["short"]),
                              "name": joined(e["name"]), "extra": joined(e["extra"]), "encoding": sorted(e["enc"]),
                              "old": ", ".join(where(k, cross_scope) for k in sorted(g["old"], key=sort_key)),
                              "new": ", ".join(where(k, cross_scope) for k in sorted(g["new"], key=sort_key))})
            moved_renames.append((sorted(g["old"], key=sort_key)[0], first))

    for okey, nkey in moved_renames:
        o, n = old[okey], new[nkey]
        short = joined(n["short"])
        if o["short"] != n["short"]:
            add("renamed-shortName", nkey, short, joined(o["short"]), joined(n["short"]))
        if o["name"] != n["name"]:
            add("renamed-name", nkey, short, joined(o["name"]), joined(n["name"]))
        if o["extra"] != n["extra"]:
            add(extra_change, nkey, short, joined(o["extra"]), joined(n["extra"]))

    # Where each id is, per edition: an entry only in one version is a new or removed parameter only
    # if its id is nowhere else in the other version (within the compared selection) for the same
    # MTG2 switch. Legacy entries count for every switch: ecCodes uses them as the last fallback.
    def by_id(entries):
        out = {}
        for k in entries:
            out.setdefault((k[0], k[4]), []).append(k)
        return out

    old_ids, new_ids = by_id(old), by_id(new)

    def elsewhere(key, ids):
        found = [k for k in ids.get((key[0], key[4]), [])
                 if k[1] == key[1] or k[2] or key[2]]
        return ", ".join(where(k, True) for k in sorted(found, key=sort_key))

    for key in sorted(set(old) | set(new), key=sort_key):
        if key in moved_keys:
            continue
        o, n = old.get(key), new.get(key)
        if o is None:
            text = "%s | %s | %s" % (joined(n["short"]), joined(n["name"]), joined(n["extra"]))
            other = elsewhere(key, old_ids)
            if other:
                add("added-entry", key, joined(n["short"]), other, text)
            else:
                add("new", key, joined(n["short"]), "", text)
            continue
        if n is None:
            text = "%s | %s | %s" % (joined(o["short"]), joined(o["name"]), joined(o["extra"]))
            other = elsewhere(key, new_ids)
            if other:
                add("removed-entry", key, joined(o["short"]), text, other)
            else:
                add("removed", key, joined(o["short"]), text, "")
            continue
        short = joined(n["short"])
        if all(o[k] == n[k] for k in ("enc", "short", "name", "extra")):
            text = "%s | %s | %s" % (joined(n["short"]), joined(n["name"]), joined(n["extra"]))
            add("common", key, short, text, text)
            continue
        if o["enc"] != n["enc"]:
            # One row per entry: encodings only in the old definitions, and only in the new ones
            add("changed-encoding", key, short, "; ".join(sorted(o["enc"] - n["enc"])),
                "; ".join(sorted(n["enc"] - o["enc"])))
        if o["short"] != n["short"]:
            add("renamed-shortName", key, short, joined(o["short"]), joined(n["short"]))
        if o["name"] != n["name"]:
            add("renamed-name", key, short, joined(o["name"]), joined(n["name"]))
        if o["extra"] != n["extra"]:
            add(extra_change, key, short, joined(o["extra"]), joined(n["extra"]))

    order = {c: i for i, c in enumerate(CHANGES)}
    # By change type, then like grib_defs (edition, legacy, switch, scope, id); stable within an entry
    diffs.sort(key=lambda d: (order[d["change"]],
                              sort_key((d["edition"], d["is_mtg2_switch"], d["is_legacy"], d["scope"], d["id"]))))
    return diffs


def sort_key(key):
    edition, sw, legacy, scope, ident = key
    try:
        ident_sort = (0, int(ident), "")
    except (TypeError, ValueError):
        ident_sort = (1, 0, str(ident))
    return (edition or 0, legacy or 0, -1 if sw is None else sw, scope or "", ident_sort)


def cell(v):
    if isinstance(v, list):  # encodings of an entry
        return "; ".join(v)
    return "" if v is None else str(v)


def available_columns(kind):
    """Column name as given to --columns -> key in the difference dicts. Names follow grib_defs."""
    f = KINDS[kind]
    return {"change": "change", "edition": "edition", "is_mtg2_switch": "is_mtg2_switch",
            "is_legacy": "is_legacy", "scope": "scope", f["id"]: "id", f["short"]: "short",
            f["name"]: "name", f["extra"]: "extra", "encoding": "encoding", "old": "old", "new": "new"}


def default_columns(kind, match):
    f = KINDS[kind]
    cols = ["change"]
    if match == "scope":
        cols += ["edition", "is_mtg2_switch", "is_legacy", "scope"]
    elif match == "switch":
        cols += ["edition", "is_mtg2_switch", "is_legacy"]
    return cols + [f["id"], f["short"], "old", "new"]


# Short table headers for long column names
HEADERS = {"is_mtg2_switch": "switch", "is_legacy": "legacy"}


def columns_for(kind, names):
    """List of (key in the difference dicts, header) for the given column names."""
    avail = available_columns(kind)
    return [(avail[n], HEADERS.get(n, n)) for n in names]


def print_table(diffs, cols, out):
    headers = [h for _, h in cols]
    rows = [[cell(d[k]) for k, _ in cols] for d in diffs]
    widths = [max([len(h)] + [len(r[i]) for r in rows]) for i, h in enumerate(headers)]
    fmt = "  ".join("%%-%ds" % w for w in widths)
    out.write((fmt % tuple(headers)).rstrip() + "\n")
    out.write((fmt % tuple("-" * w for w in widths)).rstrip() + "\n")
    for r in rows:
        out.write((fmt % tuple(r)).rstrip() + "\n")


def md_escape(s):
    return s.replace("\\", "\\\\").replace("|", "\\|")


def print_markdown(diffs, cols, out, summary):
    out.write("| " + " | ".join(h for _, h in cols) + " |\n")
    out.write("|" + "|".join("---" for _ in cols) + "|\n")
    for d in diffs:
        vals = []
        for k, _ in cols:
            v = md_escape(cell(d[k]))
            # Encodings and old/new values are code-like; keep them readable
            if ((k in ("old", "new") and d["change"] == "changed-encoding") or k == "encoding") and v:
                v = "`" + v + "`"
            vals.append(v)
        out.write("| " + " | ".join(vals) + " |\n")
    out.write("\n" + ", ".join("%s: %d" % (c, n) for c, n in summary.items()) + "\n")


def main():
    argv = sys.argv[1:]
    passthrough = []
    if "--" in argv:
        i = argv.index("--")
        argv, passthrough = argv[:i], argv[i + 1:]

    p = argparse.ArgumentParser(
        description="Compare the parameters (or chemicals) of two ecCodes definitions sets. "
                    "Based on grib_defs: the script runs grib_defs (params or chems, --show-encoding "
                    "--format json) once for the old and once for the new side and compares the outputs, "
                    "so it sees the definitions exactly as ecCodes does. Requires a grib_defs executable.",
        epilog="Arguments after '--' are passed to both grib_defs runs, e.g. "
               "'-- --paramId 13*'. Without change flags, all changes are shown. "
               "Exit status: 0 no differences, 1 differences, 2 error. --common entries do not count as differences.")
    m = p.add_argument_group("comparison mode: either two definitions folders or two grib_defs binaries")
    m.add_argument("--definition-old", metavar="DIR",
                   help="Old definitions directory (or several, separated by ':'). Requires --definition-new")
    m.add_argument("--definition-new", metavar="DIR",
                   help="New definitions directory (or several, separated by ':'). Requires --definition-old")
    p.add_argument("--kind", choices=sorted(KINDS), default="params",
                   help="grib_defs subcommand: params (default) or chems")
    m.add_argument("--grib-defs", metavar="PATH",
                   help="With definitions folders: the grib_defs executable reading both, or the directory "
                        "containing it (default: $GRIB_DEFS, else grib_defs in PATH)")
    m.add_argument("--grib-defs-old", metavar="PATH",
                   help="Old grib_defs (or its directory), using its own definitions, e.g. of an older "
                        "ecCodes installation. Requires --grib-defs-new")
    m.add_argument("--grib-defs-new", metavar="PATH",
                   help="New grib_defs (or its directory), using its own definitions. Requires --grib-defs-old")
    p.add_argument("--scope", metavar="PATTERNS",
                   help="Compare only these scopes: comma-separated wildcard patterns, "
                        "e.g. 'grib2' or 'grib2/localConcepts/*' or 'grib1,grib2'")
    p.add_argument("--exclude-scope", metavar="PATTERNS",
                   help="Leave out these scopes (comma-separated wildcard patterns), "
                        "e.g. 'grib*/localConcepts/*' for the WMO tables only")
    p.add_argument("--edition", choices=["1", "2"], help="Compare only GRIB edition 1 or 2 (params only)")
    p.add_argument("--is_mtg2_switch", choices=["0", "1", "2"],
                   help="Compare only the GRIB2 definitions of this MTG2 switch: lte33=0, def=1, chemsplit=2 "
                        "(params only)")
    p.add_argument("--is_legacy", choices=["0", "1"],
                   help="Compare only entries from *.legacy.def files (1) or only the others (0) (params only)")
    p.add_argument("--match", choices=["scope", "switch", "id"], default="scope",
                   help="scope (default): compare each id per edition, MTG2 switch, legacy flag and scope. "
                        "switch: per edition, MTG2 switch and legacy flag, taking the selected scopes as one, "
                        "e.g. --scope grib2,grib2/localConcepts/ecmf for what ecCodes uses for ECMWF data. "
                        "id: compare each id across all scopes, e.g. to list ids new to ecCodes as a whole")

    g = p.add_argument_group("changes to show (default: all differences, i.e. all except --common)")
    g.add_argument("--new-scope", action="store_true",
                   help="Scopes, e.g. a gribN/localConcepts/<centre> folder, with entries only in the new definitions")
    g.add_argument("--removed-scope", action="store_true",
                   help="Scopes with entries only in the old definitions")
    g.add_argument("--moved", action="store_true",
                   help="Entries moved between MTG2 switches or to/from legacy files, with the same "
                        "encodings (name or unit changes are listed in addition); old and new list the placements, e.g. 'switch 0, switch 1' -> 'legacy'. "
                        "Such entries are not listed under --new and --removed")
    g.add_argument("--new", action="store_true",
                   help="New parameters: ids which the old definitions do not have for this edition and MTG2 "
                        "switch, in any compared scope (legacy entries count for every switch)")
    g.add_argument("--removed", action="store_true",
                   help="Removed parameters: ids which the new definitions no longer have for this edition and "
                        "MTG2 switch, in any compared scope")
    g.add_argument("--added-entry", action="store_true",
                   help="Entries added to a scope for an id which the old definitions have in another scope "
                        "for the same switch (or as legacy), e.g. a parameter added to grib2 which was only in "
                        "grib2/localConcepts/ecmf; old lists where the id was")
    g.add_argument("--removed-entry", action="store_true",
                   help="Entries removed from a scope for an id which the new definitions still have in another "
                        "scope for the same switch (or as legacy); new lists where the id is")
    g.add_argument("--changed-encoding", action="store_true",
                   help="Entries whose set of encodings differs; one row per entry, the encodings only in the old "
                        "definitions under old and those only in the new ones under new (separated by '; ')")
    g.add_argument("--renamed-shortName", action="store_true", help="Entries whose short name differs")
    g.add_argument("--renamed-name", action="store_true", help="Entries whose name differs")
    g.add_argument("--changed-unit", action="store_true", help="params: entries whose units differ")
    g.add_argument("--changed-formula", action="store_true", help="chems: entries whose chemFormula differs")
    g.add_argument("--common", action="store_true",
                   help="Entries identical in both definitions (same names and encodings)")

    p.add_argument("--ignore-whitespace", action="store_true",
                   help="Ignore leading, trailing and repeated whitespace in names, units and formulas. "
                        "Otherwise such values are shown in double quotes, e.g. a removed trailing space")
    p.add_argument("--columns", metavar="COLUMNS",
                   help="Comma-separated output columns, in this order. Valid: change,edition,is_mtg2_switch,"
                        "is_legacy,scope,paramId,shortName,name,units,encoding,old,new (chems: chemId,chemShortName,"
                        "chemName,chemFormula instead). Default: change,edition,is_mtg2_switch,is_legacy,scope,"
                        "paramId,shortName,old,new (without scope for --match switch, without edition, is_mtg2_switch, "
                        "is_legacy and scope for --match id). "
                        "name, units and encoding show the new values, or the old ones for removed entries; "
                        "encoding lists all encodings of the entry, separated by '; ' (an array in JSON). "
                        "Rows equal in the shown columns are printed once")
    p.add_argument("--format", choices=["table", "markdown", "json"], default="table",
                   help="Output format (default: table)")
    p.add_argument("--summary", action="store_true", help="Print only the number of differences per change type")
    args = p.parse_args(argv)
    global IGNORE_WHITESPACE
    IGNORE_WHITESPACE = args.ignore_whitespace

    # Restrictions applied by grib_defs itself, in both runs
    for opt in ("edition", "is_mtg2_switch", "is_legacy"):
        if getattr(args, opt) is not None:
            if args.kind == "chems":
                p.error("--%s applies to --kind params" % opt)
            passthrough = ["--" + opt, getattr(args, opt)] + passthrough

    avail = available_columns(args.kind)
    if args.columns:
        col_names = [c.strip() for c in args.columns.split(",") if c.strip()]
        bad = [c for c in col_names if c not in avail]
        if bad or not col_names:
            p.error("invalid column(s) %s for --kind %s. Valid: %s" % (",".join(bad), args.kind, ",".join(avail)))
    else:
        col_names = default_columns(args.kind, args.match)
    cols = columns_for(args.kind, col_names)

    if args.kind == "params" and args.changed_formula:
        p.error("--changed-formula applies to --kind chems")
    if args.kind == "chems" and args.changed_unit:
        p.error("--changed-unit applies to --kind params")

    defs_mode = args.definition_old is not None or args.definition_new is not None
    bin_mode = args.grib_defs_old is not None or args.grib_defs_new is not None
    if defs_mode and bin_mode:
        p.error("definitions folders (--definition-old/--definition-new) and binaries "
                "(--grib-defs-old/--grib-defs-new) cannot be mixed")
    if bin_mode and args.grib_defs:
        p.error("--grib-defs applies to definitions folders; with binaries use --grib-defs-old and --grib-defs-new")
    if defs_mode and not (args.definition_old and args.definition_new):
        p.error("both --definition-old and --definition-new are required")
    if bin_mode and not (args.grib_defs_old and args.grib_defs_new):
        p.error("both --grib-defs-old and --grib-defs-new are required")
    if not defs_mode and not bin_mode:
        p.error("give either --definition-old and --definition-new, or --grib-defs-old and --grib-defs-new")

    for d in (args.definition_old, args.definition_new):
        for part in (d or "").split(os.pathsep):
            if part and not os.path.isdir(part):
                p.error("not a directory: %s" % part)

    def resolve(path):
        path = path or os.environ.get("GRIB_DEFS") or shutil.which("grib_defs")
        if not path:
            p.error("grib_defs not found; use --grib-defs or set GRIB_DEFS")
        if os.path.isdir(path):
            path = os.path.join(path, "grib_defs")
        if not (os.path.isfile(path) and os.access(path, os.X_OK)):
            p.error("grib_defs is not an executable file: %s" % path)
        return os.path.realpath(path)

    if bin_mode:
        grib_defs_old, grib_defs_new = resolve(args.grib_defs_old), resolve(args.grib_defs_new)
        if grib_defs_old == grib_defs_new:
            p.error("--grib-defs-old and --grib-defs-new are the same executable: %s" % grib_defs_old)
    else:
        grib_defs_old = grib_defs_new = resolve(args.grib_defs)

    applicable = [c for c in CHANGES if c != ("changed-formula" if args.kind == "params" else "changed-unit")]
    wanted = {c for c in applicable if getattr(args, c.replace("-", "_"))} or set(applicable) - NOT_DIFFERENCES

    # Both definitions sets are scanned in parallel
    po, co = run_grib_defs(grib_defs_old, args.kind, args.definition_old, passthrough)
    pn, cn = run_grib_defs(grib_defs_new, args.kind, args.definition_new, passthrough)
    old_rows, new_rows = collect(po, co), collect(pn, cn)
    include, exclude = patterns(args.scope), patterns(args.exclude_scope)
    all_scopes = {r.get("scope") or "" for r in old_rows + new_rows}
    for pat in include + exclude:
        if not any(fnmatch.fnmatchcase(sc, pat) for sc in all_scopes):
            sys.stderr.write("Warning: scope pattern '%s' matches no scope in either definitions set\n" % pat)
    old_rows = scope_filter(old_rows, include, exclude)
    new_rows = scope_filter(new_rows, include, exclude)

    old = index_rows(old_rows, args.kind, args.match)
    new = index_rows(new_rows, args.kind, args.match)
    diffs = compare(old, new, wanted, args.kind, old_rows, new_rows)

    if args.columns:
        # Like grib_defs: rows that are equal in the shown columns are printed once
        seen, unique = set(), []
        for d in diffs:
            row = tuple(json.dumps(d[k], sort_keys=True) for k, _ in cols)
            if row not in seen:
                seen.add(row)
                unique.append(d)
        diffs_out = unique
    else:
        diffs_out = diffs

    summary = {c: sum(1 for d in diffs_out if d["change"] == c) for c in applicable if c in wanted}
    out = sys.stdout
    if args.summary:
        if args.format == "json":
            json.dump(summary, out, indent=2)
            out.write("\n")
        else:
            for c, n in summary.items():
                out.write("%-18s %d\n" % (c, n))
    elif args.format == "json":
        json.dump([{name: d[key] for name, key in zip(col_names, (k for k, _ in cols))} for d in diffs_out], out, indent=1)
        out.write("\n")
    elif not diffs_out:
        sys.stderr.write("No differences found.\n" if "common" not in wanted else "No matching entries found.\n")
    elif args.format == "markdown":
        print_markdown(diffs_out, cols, out, summary)
    else:
        print_table(diffs_out, cols, out)
        out.write("\n" + ", ".join("%s: %d" % (c, n) for c, n in summary.items()) + "\n")

    return 1 if any(d["change"] not in NOT_DIFFERENCES for d in diffs) else 0


if __name__ == "__main__":
    sys.exit(main())

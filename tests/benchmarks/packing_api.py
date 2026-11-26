#!/usr/bin/env python3
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
"""An overview of the speed of CCSDS and simple packing through the ecCodes API.

Runs tests/benchmarks/packing_api (built in <build>/tests) with the implementations:

  CCSDS:          legacy (ECCODES_USE_SPAEC=0, libaec), new with ECCODES_SPAEC_SELECTOR=search
                  and with =estimate (ECCODES_USE_SPAEC=1)
  simple packing: legacy (ECCODES_USE_SP=0), new (ECCODES_USE_SP=1)

and prints the tables of the READMEs of projects/spaec and projects/simple_packing (MiB/s of the
array passed to ecCodes, millions of values per second, speed-up against legacy). It also checks
that legacy and new write byte-identical messages (search for CCSDS).

usage: packing_api.py --program <build>/tests/packing_api --ccsds <file> --simple <file>
                      [--compiler NAME]

<file>: GRIB messages with CCSDS packing (grid_ccsds) and with simple packing (grid_simple), e.g.
all messages of a forecast concatenated in one file. Set ECCODES_DEFINITION_PATH if ecCodes is not
installed. Exit status 1 if a check fails.
"""

import argparse
import filecmp
import os
import re
import subprocess
import sys
import tempfile

CALLS = [
    "codes_get_double_array",
    "codes_get_float_array",
    "codes_set_double_array",
    "codes_set_float_array",
]
LINE = re.compile(
    r"^(\w+) values=(\d+) seconds=([\d.]+) MiB/s=(\d+) Mvalues/s=(\d+)$", re.MULTILINE
)


def run(program, grib, out_prefix, env):
    """Runs the program with the environment; returns {call: (MiB/s, Mvalues/s)}"""
    result = subprocess.run(
        [program, grib, out_prefix],
        check=False,
        env=dict(os.environ, **env),
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        sys.exit(f"packing_api failed ({env}): {result.stderr.strip()}")
    return {
        m.group(1): (float(m.group(4)), float(m.group(5)))
        for m in LINE.finditer(result.stdout)
    }


def cell(r):
    return f"{r[0]:,.0f} | {r[1]:,.0f}"


def main():
    parser = argparse.ArgumentParser(
        description="An overview of the speed of CCSDS and simple packing through the ecCodes API"
    )
    parser.add_argument("--program", required=True, help="the packing_api program")
    parser.add_argument("--ccsds", required=True, help="GRIB file with CCSDS packing")
    parser.add_argument("--simple", required=True, help="GRIB file with simple packing")
    parser.add_argument(
        "--compiler", default="", help="a column with this name (e.g. GCC)"
    )
    args = parser.parse_args()
    compiler = f" {args.compiler} |" if args.compiler else ""
    compiler_header = (" Compiler |", "---|") if args.compiler else ("", "")

    failures = 0
    with tempfile.TemporaryDirectory() as tmp:

        def out(name):
            return os.path.join(tmp, name)

        cc = {
            "legacy": run(
                args.program, args.ccsds, out("cc_legacy"), {"ECCODES_USE_SPAEC": "0"}
            ),
            "search": run(
                args.program,
                args.ccsds,
                out("cc_search"),
                {"ECCODES_USE_SPAEC": "1", "ECCODES_SPAEC_SELECTOR": "search"},
            ),
            "estimate": run(
                args.program,
                args.ccsds,
                out("cc_estimate"),
                {"ECCODES_USE_SPAEC": "1", "ECCODES_SPAEC_SELECTOR": "estimate"},
            ),
        }
        sp = {
            "legacy": run(
                args.program, args.simple, out("sp_legacy"), {"ECCODES_USE_SP": "0"}
            ),
            "new": run(
                args.program, args.simple, out("sp_new"), {"ECCODES_USE_SP": "1"}
            ),
        }
        for a, b in (("cc_legacy", "cc_search"), ("sp_legacy", "sp_new")):
            for kind in ("d", "f"):
                if not filecmp.cmp(
                    out(f"{a}.{kind}.grib"), out(f"{b}.{kind}.grib"), shallow=False
                ):
                    print(
                        f"FAIL: the messages of {a} and {b} differ (encoded from {'doubles' if kind == 'd' else 'floats'})",
                        file=sys.stderr,
                    )
                    failures += 1
        extra = 100.0 * (
            os.path.getsize(out("cc_estimate.d.grib"))
            / os.path.getsize(out("cc_search.d.grib"))
            - 1.0
        )

    print("CCSDS (grid_ccsds)\n")
    print(
        f"| Call |{compiler_header[0]} Legacy MiB/s | Legacy Mvalues/s | Search MiB/s | Search Mvalues/s | Speed-up |"
        " Estimate MiB/s | Estimate Mvalues/s | Speed-up |"
    )
    print(f"|---|{compiler_header[1]}---|---|---|---|---|---|---|---|")
    for call in CALLS:
        legacy, search, estimate = (
            cc["legacy"][call],
            cc["search"][call],
            cc["estimate"][call],
        )
        est = (
            f"{cell(estimate)} | {estimate[0] / legacy[0]:.1f}×"
            if "set" in call
            else "— | — | —"
        )
        print(
            f"| `{call}` |{compiler} {cell(legacy)} | {cell(search)} | {search[0] / legacy[0]:.1f}× | {est} |"
        )
    print(f"\nEstimate writes {extra:+.3f} % bytes against Search.\n")
    print("Simple packing (grid_simple)\n")
    print(
        f"| Call |{compiler_header[0]} Legacy MiB/s | Legacy Mvalues/s | New MiB/s | New Mvalues/s | Speed-up |"
    )
    print(f"|---|{compiler_header[1]}---|---|---|---|---|")
    for call in CALLS:
        legacy, new = sp["legacy"][call], sp["new"][call]
        print(
            f"| `{call}` |{compiler} {cell(legacy)} | {cell(new)} | {new[0] / legacy[0]:.1f}× |"
        )
    if failures:
        sys.exit(1)


if __name__ == "__main__":
    main()

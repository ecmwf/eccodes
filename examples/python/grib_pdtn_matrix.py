# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

# Python Implementation: grib_pdtn_matrix
#
# Description: Demonstrate the PDTN matrix auto-selection mechanism.
#   When a key is not present in the current Product Definition Template,
#   ecCodes automatically selects a compatible template that supports it.
#
#   Requires: ECCODES_PDTN_MATRIX_ENABLE=1
#

import os
import sys
from eccodes import *

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
# Walk up to workspace root (eccodes/examples/python -> eccodes -> workspace root)
_WORKSPACE = os.path.normpath(os.path.join(_SCRIPT_DIR, "..", "..", ".."))
INPUT  = os.path.join(_WORKSPACE, "sample.grib2")
OUTPUT = os.path.join(_WORKSPACE, "out_pdtn_matrix.grib2")


def example():
    if os.environ.get("ECCODES_PDTN_MATRIX_ENABLE", "0") == "0":
        print("Set ECCODES_PDTN_MATRIX_ENABLE=1 to enable the PDTN matrix mechanism")
        sys.exit(1)

    fin = open(INPUT, "rb")
    fout = open(OUTPUT, "wb")

    gid = codes_grib_new_from_file(fin)

    pdtn_before = codes_get(gid, "productDefinitionTemplateNumber", int)
    print(f"productDefinitionTemplateNumber before: {pdtn_before}")

    # typeOfStatisticalProcessing is not defined in PDT 0.
    # The matrix will select PDT 8 (time-range statistically processed deterministic).
    codes_set(gid, "typeOfStatisticalProcessing", 0)  # 0 = Average

    pdtn_after = codes_get(gid, "productDefinitionTemplateNumber", int)
    tosp       = codes_get(gid, "typeOfStatisticalProcessing", int)
    print(f"productDefinitionTemplateNumber after:  {pdtn_after}")
    print(f"typeOfStatisticalProcessing:            {tosp}")

    assert pdtn_after != pdtn_before, "Expected a PDTN switch"
    assert tosp == 0

    codes_write(gid, fout)
    codes_release(gid)

    fin.close()
    fout.close()

    print(f"Written to {OUTPUT}")


def main():
    try:
        example()
    except CodesInternalError as err:
        print(f"Error: {err.msg}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())

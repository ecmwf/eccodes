#
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

import traceback
import sys

from eccodes import *

INPUT = '../../data/tigge/tigge_ecmf_pl_t.grib'
OUTPUT = 'out.p_set_missing.grib'
VERBOSE = 1  # verbose error reporting


def example():
    fin = open(INPUT)
    fout = open(OUTPUT, 'w')

    gid = codes_grib_new_from_file(fin)

    codes_set_long(gid, "scaledValueOfFirstFixedSurface", 15)
    codes_set_long(gid, "scaleFactorOfFirstFixedSurface", 1)
    level = codes_get_double(gid, "level")
    assert(level == 1.5)

    # set type of level to surface
    codes_set(gid, 'typeOfFirstFixedSurface', 'sfc')
    codes_set_missing(gid, 'scaleFactorOfFirstFixedSurface')
    codes_set_missing(gid, 'scaledValueOfFirstFixedSurface')

    codes_write(gid, fout)

    codes_release(gid)
    fin.close()
    fout.close()


def main():
    try:
        example()
    except CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + '\n')

        return 1

if __name__ == "__main__":
    sys.exit(main())

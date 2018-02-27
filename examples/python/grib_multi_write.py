#
# Copyright 2005-2018 ECMWF.
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

INPUT = '../../data/sample.grib2'
OUTPUT = 'out.mw.grib'
VERBOSE = 1  # verbose error reporting


def example():
    fin = open(INPUT)
    fout = open(OUTPUT, 'w')

    gid = codes_grib_new_from_file(fin)

    mgid = codes_grib_multi_new()

    for step in range(12, 132, 12):
        codes_set(gid, "step", step)
        codes_grib_multi_append(gid, 4, mgid)

    codes_grib_multi_write(mgid, fout)

    codes_grib_multi_release(mgid)
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

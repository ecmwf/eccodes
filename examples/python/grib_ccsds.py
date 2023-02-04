# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

import sys
import traceback

from eccodes import *

INPUT = "../../data/ccsds.grib2"
VERBOSE = 1  # verbose error reporting


def example():
    f = open(INPUT, "rb")

    keys = [
        "ccsdsFlags",
        "ccsdsBlockSize",
        "bitsPerValue",
    ]

    while 1:
        gid = codes_grib_new_from_file(f)
        if gid is None:
            break

        for key in keys:
            try:
                print("  %s: %s" % (key, codes_get(gid, key)))
            except CodesInternalError as err:
                print('Error with key="%s" : %s' % (key, err.msg))

        codes_release(gid)

    f.close()


def main():
    try:
        example()
    except CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + "\n")

        return 1


if __name__ == "__main__":
    sys.exit(main())

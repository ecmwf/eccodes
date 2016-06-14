#
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

import traceback
import sys

from eccodes import *

VERBOSE = 1  # verbose error reporting


def example(INPUT):
    f = open(INPUT)

    while 1:
        gid = codes_grib_new_from_file(f)
        if gid is None:
            break

        iterid = codes_grib_iterator_new(gid, 0)

        missingValue = codes_get_double(gid, "missingValue")

        i = 0
        while 1:
            result = codes_grib_iterator_next(iterid)
            if not result:
                break

            [lat, lon, value] = result

            sys.stdout.write("- %d - lat=%.6e lon=%.6e value=" % (i, lat, lon))

            if value == missingValue:
                print "missing"
            else:
                print "%.6f" % value

            i += 1

        codes_grib_iterator_delete(iterid)
        codes_release(gid)

    f.close()


def main():
    try:
        example(sys.argv[1])
    except CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + '\n')

        return 1

if __name__ == "__main__":
    sys.exit(main())

#
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

from __future__ import print_function
import traceback
import sys

from eccodes import *

INPUT = '../../data/regular_latlon_surface.grib1'
VERBOSE = 1  # verbose error reporting


def example():
    f = open(INPUT, 'rb')
    gid = codes_grib_new_from_file(f)

    values = codes_get_values(gid)
    num_vals = len(values)
    for i in range(num_vals):
        print("%d %.10e" % (i + 1, values[i]))

    print('%d values found in %s' % (num_vals, INPUT))

    for key in ('max', 'min', 'average'):
        print('%s=%.10e' % (key, codes_get(gid, key)))

    # Example of accessing specific elements from data values
    # Get first, middle and last elements
    indexes = [0, int(num_vals/2), num_vals-1]
    elems = codes_get_double_elements(gid, 'values', indexes)

    codes_release(gid)
    f.close()


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

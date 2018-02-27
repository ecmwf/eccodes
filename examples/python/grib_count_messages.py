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

from __future__ import print_function
import traceback
import sys

from eccodes import *

INPUT = '../../data/tigge_pf_ecmwf.grib2'
VERBOSE = 1  # verbose error reporting


def example():
    f = open(INPUT)

    mcount = codes_count_in_file(f)
    gid_list = [codes_grib_new_from_file(f) for i in range(mcount)]

    f.close()

    keys = [
        'Ni',
        'Nj',
        'latitudeOfFirstGridPointInDegrees',
        'longitudeOfFirstGridPointInDegrees',
        'latitudeOfLastGridPointInDegrees',
        'longitudeOfLastGridPointInDegrees',
        'jDirectionIncrementInDegrees',
        'iDirectionIncrementInDegrees',
    ]

    for i in range(mcount):
        gid = gid_list[i]

        print("processing message number", i + 1)

        for key in keys:
            print('%s=%g' % (key, codes_get(gid, key)))

        print('There are %d, average is %g, min is %g, max is %g' % (
            codes_get_size(gid, 'values'),
            codes_get(gid, 'average'),
            codes_get(gid, 'min'),
            codes_get(gid, 'max')
        ))

        print('-' * 100)

        codes_release(gid)


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

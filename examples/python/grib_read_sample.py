# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

#
# Python implementation: grib_read_sample
#
# Description: read key values from a GRIB sample message.
#

from __future__ import print_function
import traceback
import sys

from eccodes import *

VERBOSE = 1  # verbose error reporting


def example(INPUT):
    # open GRIB file
    f = open(INPUT, 'rb')

    # These keys should be in the sample files
    keys = [
        'identifier',
        'editionNumber',
        'year',
        'month',
        'latitudeOfFirstGridPointInDegrees',
        'longitudeOfFirstGridPointInDegrees',
        'bitsPerValue',
        'stepType',
        'packingType'
    ]

    cnt = 0

    # Loop for the messages in the file
    while 1:
        gid = codes_grib_new_from_file(f)
        if gid is None:
            break

        print("message: %s" % cnt)

        for key in keys:
            print('  %s=%s' % (key, codes_get(gid, key)))

        cnt += 1

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

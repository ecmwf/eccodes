# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

#
# Python implementation: bufr_read_sample
#
# Description: read key values from a BUFR sample message.
#

from __future__ import print_function
import traceback
import sys

from eccodes import *

VERBOSE = 1  # verbose error reporting

def get_key_value(msgid, key):
    v = codes_get(msgid, key)
    if v == CODES_MISSING_DOUBLE or v == CODES_MISSING_LONG:
        v = 'MISSING'
    return v


def example(INPUT):
    # open BUFR file
    f = open(INPUT, 'rb')

    # These keys should be in the sample files
    keys = [
        'editionNumber',
        'unexpandedDescriptors',
        'blockNumber',
        'stationNumber',
        'verticalSignificanceSurfaceObservations',
        'latitude',
        'longitude',
        '24HourPressureChange',
        'horizontalVisibility',
        '#1#cloudAmount',  # cloud amount (low and mid level)
        '#1#heightOfBaseOfCloud',
        '#1#cloudType',  # cloud type (low clouds)
        '#2#cloudType',  # cloud type (middle clouds)
        '#3#cloudType'  # cloud type (highclouds)
    ]

    cnt = 0

    # Loop for the messages in the file
    while 1:
        bufr = codes_bufr_new_from_file(f)
        if bufr is None:
            break

        print("message: %s" % cnt)

        codes_set(bufr, 'unpack', 1)

        for key in keys:
            print('  %s=%s' % (key, get_key_value(bufr, key)))

        cnt += 1

        codes_release(bufr)

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

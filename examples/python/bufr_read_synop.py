# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

#
# Python implementation: bufr_read_synop
#
# Description: how to read data values from BUFR messages.
#

# Please note that SYNOP reports can be encoded in various ways in BUFR.
# Therefore the code below might not work directly for other types of SYNOP
# messages than the one used in the example. It is advised to use bufr_dump to
# understand the structure of the messages.

from __future__ import print_function
import traceback
import sys

from eccodes import *

INPUT = '../../data/bufr/syno_multi.bufr'
VERBOSE = 1  # verbose error reporting


def example():
    # open bufr file
    f = open(INPUT)

    # define the keys to be printed
    keys = [
        'blockNumber',
        'stationNumber',
        'latitude',
        'longitude',
        'airTemperatureAt2M',
        'dewpointTemperatureAt2M',
        'windSpeedAt10M',
        'windDirectionAt10M',
        '#1#cloudAmount',  # cloud amount (low and mid level)
        '#1#heightOfBaseOfCloud',
        '#1#cloudType',  # cloud type (low clouds)
        '#2#cloudType',  # cloud type (middle clouds)
        '#3#cloudType'  # cloud type (highclouds)
    ]

    # The cloud information is stored in several blocks in the
    # SYNOP message and the same key means a different thing in different
    # parts of the message. In this example we will read the first
    # cloud block introduced by the key
    # verticalSignificanceSurfaceObservations=1.
    # We know that this is the first occurrence of the keys we want to
    # read so in the list above we used the # (occurrence) operator
    # accordingly.

    cnt = 0

    # loop for the messages in the file
    while 1:
        # get handle for message
        bufr = codes_bufr_new_from_file(f)
        if bufr is None:
            break

        print("message: %s" % cnt)

        # we need to instruct ecCodes to expand all the descriptors
        # i.e. unpack the data values
        codes_set(bufr, 'unpack', 1)

        # print the values for the selected keys from the message
        for key in keys:
            try:
                print('  %s: %s' % (key, codes_get(bufr, key)))
            except CodesInternalError as err:
                print('Error with key="%s" : %s' % (key, err.msg))

        cnt += 1

        # delete handle
        codes_release(bufr)

    # close the file
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

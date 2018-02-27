# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

#
# Python implementation: bufr_attributes
#
# Description: how to read attributes of keys in BUFR messages.
#
#

from __future__ import print_function
import traceback
import sys
from eccodes import *

INPUT = '../../data/bufr/syno_multi.bufr'
VERBOSE = 1  # verbose error reporting


def example():
    # open bufr file
    f = open(INPUT)

    cnt = 0

    # define the attributes to be printed (see BUFR code table B)
    attrs = [
        'code',
        'units',
        'scale',
        'reference',
        'width'
    ]

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

        # --------------------------------------------------------------
        # We will read the value and all the attributes available for
        # the 2m temperature.
        # --------------------------------------------------------------
        # get the value
        key = 'airTemperatureAt2M'
        try:
            print('  %s: %s' % (key, codes_get(bufr, key)))
        except CodesInternalError as err:
            print('Error with key="%s" : %s' % (key, err.msg))

        # print the values of the attributes of the key. Attributes themselves
        # are keys as well. Their name is constructed like:
        # keyname->attributename
        for attr in attrs:
            key = 'airTemperatureAt2M' + "->" + attr
            try:
                print('  %s: %s' % (key, codes_get(bufr, key)))
            except CodesInternalError as err:
                print('Error with key="%s" : %s' % (key, err.msg))

        # ------------------------------------------------------------------
        # The 2m temperature data element in this message has an associated
        # field: percentConfidence. Its value and attributes can be accessed
        # in a similar manner as was shown above for 2m temperature.
        # ------------------------------------------------------------------

        # get the value
        key = 'airTemperatureAt2M->percentConfidence'
        try:
            print('  %s: %s' % (key, codes_get(bufr, key)))
        except CodesInternalError as err:
            print('Error with key="%s" : %s' % (key, err.msg))

        # print the values of the attributes of the key.
        for attr in attrs:
            key = 'airTemperatureAt2M->percentConfidence' + "->" + attr
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

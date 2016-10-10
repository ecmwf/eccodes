# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

#
# Python implementation: bufr_get_keys
#
# Description: how to read values of different type of keys from BUFR messages.
#
#

import traceback
import sys

from eccodes import *

INPUT = '../../data/bufr/syno_multi.bufr'
VERBOSE = 1  # verbose error reporting


def example():

    # open bufr file
    f = open(INPUT)

    cnt = 0

    # loop for the messages in the file
    while 1:
        # get handle for message
        bufr = codes_bufr_new_from_file(f)
        if bufr is None:
            break

        print "message: %s" % cnt

        # we need to instruct ecCodes to expand all the descriptors
        # i.e. unpack the data values
        codes_set(bufr, 'unpack', 1)

        # ---------------------------------------------
        # get values for keys holding a single value
        # ---------------------------------------------
        # Native type integer
        key = 'blockNumber'

        try:
            print '  %s: %s' % (key, codes_get(bufr, key))
        except CodesInternalError as err:
            print 'Error with key="%s" : %s' % (key, err.msg)

        # Native type integer
        key = 'stationNumber'
        try:
            print '  %s: %s' % (key, codes_get(bufr, key))
        except CodesInternalError as err:
            print 'Error with key="%s" : %s' % (key, err.msg)

        # Native type float
        key = 'airTemperatureAt2M'
        try:
            print '  %s: %s' % (key, codes_get(bufr, key))
        except CodesInternalError as err:
            print 'Error with key="%s" : %s' % (key, err.msg)

        # Native type string
        key = 'typicalDate'
        try:
            print '  %s: %s' % (key, codes_get(bufr, key))
        except CodesInternalError as err:
            print 'Error with key="%s" : %s' % (key, err.msg)

        # --------------------------------
        # get values for an array
        # --------------------------------
        # Native type integer
        key = 'bufrdcExpandedDescriptors'

        # get size
        num = codes_get_size(bufr, key)
        print '  size of %s is: %s' % (key, num)

        # get values
        values = codes_get_array(bufr, key)
        for i in xrange(len(values)):
            print "   %d %06d" % (i + 1, values[i])

        # Native type float
        key = 'numericValues'

        # get size
        num = codes_get_size(bufr, key)
        print '  size of %s is: %s' % (key, num)

        # get values
        values = codes_get_array(bufr, key)
        for i in xrange(len(values)):
            print "   %d %.10e" % (i + 1, values[i])

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

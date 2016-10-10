# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

#
# Python implementation: bufr_subset
#
# Description: how to read data values from a given subset of a BUFR message.
#
#

import traceback
import sys

from eccodes import *

INPUT = '../../data/bufr/synop_multi_subset.bufr'
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

        # find out the number of subsets
        key = 'numberOfSubsets'
        numberOfSubsets = codes_get(bufr, 'numberOfSubsets')
        print ' %s: %d' % (key, numberOfSubsets)

        # loop over the subsets
        for i in range(1, numberOfSubsets + 1):

            # read and print some data values

            key = '/subsetNumber=%d/blockNumber' % i
            print key
            val = codes_get_long(bufr, key)
            print '  %s= %d' % (key, val)

            key = '/subsetNumber=%d/stationNumber' % i
            val = codes_get_long(bufr, key)
            print '  %s: %d' % (key, val)

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

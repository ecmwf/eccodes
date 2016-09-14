# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

#
# Python implementation:  bufr_read_scatterometer
#
# Description: how to read data for a given beam from scatterometer BUFR
# messages.
#
# Please note that scatterometer data can be encoded in various ways in BUFR.
# Therefore the code below might not work directly for other types of messages
# than the one used in the example. It is advised to use bufr_dump first to
# understand the structure of these messages.
#


import traceback
import sys

from eccodes import *

INPUT = '../../data/bufr/asca_139.bufr'
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

        # The BUFR file contains a single message with 2016 subsets in a
        # compressed form. It means each subset has exactly the same structure:
        # they store one location with several beams and one backscatter value
        # in each beam.
        #
        # To print the backScatter values for beamIdentifier=2 from all the
        # subsets we will simply access the key by condition (see below)

        # Get the total number of subsets.
        numObs = codes_get(bufr, "numberOfSubsets")

        print '  Number of values: %ld' % (numObs)

        # Get latitude (for all the subsets)
        lat = codes_get_array(bufr, "latitude")

        # Get longitude (for all the subsets)
        lon = codes_get_array(bufr, "longitude")

        # Get backScatter for beam two. We use an access by condition for this
        # key (for all the subsets).
        bscat = codes_get_array(bufr, "/beamIdentifier=2/backscatter")

        # Check that all arrays are same size
        if len(lat) != numObs or len(lon) != numObs or len(bscat) != numObs:
            print 'inconsistent array dimension'
            return 1

        # Print the values
        print "pixel  lat    lon    backscatter"
        print "-------------------------------"

        for i in xrange(numObs):
            print "%3d %.2f %.2f %.2f" % (i + 1, lat[i], lon[i], bscat[i])

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

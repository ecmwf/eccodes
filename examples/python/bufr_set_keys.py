# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

#
# Python implementation: bufr_set_keys
#
# Description: how to set different type of keys in BUFR messages.
#
#

import traceback
import sys

from eccodes import *

INPUT='../../data/bufr/syno_multi.bufr'
OUTPUT='bufr_set_keys_test_p.tmp.bufr'
VERBOSE=1 # verbose error reporting

def example():

    # open bufr file
    fin = open(INPUT)

    # open otput bufr file
    fout = open(OUTPUT,'w')

    cnt=0

    # loop for the messages in the file
    while 1:

        # get handle for message
        gid = codes_bufr_new_from_file(fin)
        if gid is None: break

        print "message: %s" % cnt

        # we need to instruct ecCodes to expand all the descriptors
        # i.e. unpack the data values
        #codes_set(gid,'unpack',1)

        # This is the place where you may wish to modify the message
        # E.g. we change the centre

        # set centre
        val=222
        print '  set bufrHeaderCentre to: %d' % val

        key='bufrHeaderCentre'
        if not codes_is_defined(gid,key):
            raise Exception("Key: " + key + " was not defined")
        codes_set(gid,key,val)

        #check bufrHeaderCentre's value
        print '  %s''s new value is: %d' % (key,codes_get(gid,key))

        # write modified message to output
        codes_write(gid,fout)

        cnt+=1

        # delete handle
        codes_release(gid)

    fin.close()
    fout.close()


def main():
    try:
        example()
    except CodesInternalError,err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            print >>sys.stderr,err.msg

        return 1

if __name__ == "__main__":
    sys.exit(main())

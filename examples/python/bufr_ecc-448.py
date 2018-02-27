# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

from __future__ import print_function
import traceback
import sys

from eccodes import *

INPUT = '../../data/bufr/syno_1.bufr'
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

        print("message: %s" % cnt)

        # ECC-448: create a new BUFR handle from the message
        #          of the original
        the_message = codes_get_message(bufr)
        newbufr = codes_new_from_message(the_message)

        codes_set(newbufr, 'unpack', 1)

        # get BUFR key iterator
        iterid = codes_bufr_keys_iterator_new(newbufr)

        # loop over the keys
        while codes_bufr_keys_iterator_next(iterid):
            # print key name
            keyname = codes_bufr_keys_iterator_get_name(iterid)
            print("  %s" % keyname)

        # delete the key iterator
        codes_bufr_keys_iterator_delete(iterid)

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

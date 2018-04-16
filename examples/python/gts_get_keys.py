# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

#
# Python implementation: gts_get_keys
#
# Description: how to read values of different type of keys from GTS messages.
#
#

from __future__ import print_function
import traceback
import sys

from eccodes import *

INPUT = '../../data/gts/EGRR20150317121020_00493212.DAT'
VERBOSE = 1  # verbose error reporting


def example():
    # open GTS file
    f = open(INPUT)

    cnt = 0

    # loop for the messages in the file
    while 1:
        # get handle for message
        gid = codes_gts_new_from_file(f)
        if gid is None:
            break

        print("message: %s" % cnt)

        # ---------------------------------------------
        # get values for keys holding a single value
        # ---------------------------------------------
        keys = ['TT', 'AA', 'II', 'CCCC', 'YY', 'GG', 'gg', 'BBB']

        for key in keys:
            try:
                print('  %s: %s' % (key, codes_get(gid, key)))
            except CodesInternalError as err:
                print('Error with key="%s" : %s' % (key, err.msg))

        cnt += 1

        # delete handle
        codes_release(gid)

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

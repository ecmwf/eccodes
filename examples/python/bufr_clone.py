# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

#
# Python implementation: bufr_clone
#
# Description: how to create a new BUFR message by cloning
# an existing message.
#

from __future__ import absolute_import

import sys
import traceback

from eccodes import *

INPUT = "../../data/bufr/syno_1.bufr"
OUTPUT = "bufr_clone_test_p.clone.bufr"
VERBOSE = 1  # verbose error reporting


def example():
    # open BUFR file
    fin = open(INPUT, "rb")

    # open output BUFR file
    fout = open(OUTPUT, "wb")

    # get handle for message
    bufr = codes_bufr_new_from_file(fin)

    # create several clones of this message and alter them
    # in different ways
    for centre in range(0, 3):
        # clone the message
        clone_id = codes_clone(bufr)

        # this is the place where you may wish to modify the clone
        codes_set(clone_id, "bufrHeaderCentre", centre)

        # write the cloned message to a file
        codes_write(clone_id, fout)

        # release the clone's handle
        codes_release(clone_id)

    # release the source's handle
    codes_release(bufr)

    fin.close()
    fout.close()


def main():
    try:
        example()
    except CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + "\n")

        return 1


if __name__ == "__main__":
    sys.exit(main())

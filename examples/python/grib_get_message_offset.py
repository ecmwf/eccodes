#
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

# Python implementation: get_message_offset
#
# Description: how to get the message offset
#

import traceback
import sys
from eccodes import *

VERBOSE = 1  # verbose error reporting


def example():
    if len(sys.argv) < 2:
        print >>sys.stderr, 'Usage: ', sys.argv[0], ' file'
        sys.exit(1)

    f = open(sys.argv[1])
    while 1:
        id = codes_grib_new_from_file(f)
        if id is None:
            break

        print codes_get_message_offset(id)

        codes_release(id)

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

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

import traceback
import sys

from eccodes import *

VERBOSE = 1  # verbose error reporting


def example():
    f = open(sys.argv[1])
    out = open(sys.argv[2], 'w')

    while 1:
        gid = codes_grib_new_from_file(f)
        if gid is None:
            break

        message = codes_get_message(gid)

        newgid = codes_new_from_message(message)
        codes_write(newgid, out)
        codes_release(newgid)

        codes_release(gid)

    out.close()
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

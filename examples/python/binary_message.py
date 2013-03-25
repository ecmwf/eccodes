#
# Copyright 2005-2012 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

import traceback
import sys

from eccode import *

VERBOSE=1 # verbose error reporting

def example():
    f = open(sys.argv[1])
    out = open(sys.argv[2],'w')

    while 1:
        gid = grib_new_from_file(f)
        if gid is None: break

        message = get_message(gid)

        newgid = new_from_message(message)
        write(newgid,out)
        release(newgid)

        release(gid)

    out.close()
    f.close()

def main():
    try:
        example()
    except InternalError,err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            print >>sys.stderr,err.msg

        return 1

if __name__ == "__main__":
    sys.exit(main())

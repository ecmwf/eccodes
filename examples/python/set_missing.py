#
# Copyright 2005-2013 ECMWF.
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

INPUT='../../data/tigge/tigge_ecmf_pl_t.grib'
OUTPUT='out.grib'
VERBOSE=1 # verbose error reporting

def example():
    fin = open(INPUT)
    fout = open(OUTPUT,'w')

    gid = new_from_file(fin)

    # set type of level to surface
    set(gid,'typeOfFirstFixedSurface','sfc')
    set_missing(gid,'scaleFactorOfFirstFixedSurface')
    set_missing(gid,'scaledValueOfFirstFixedSurface')

    write(gid,fout)

    release(gid)
    fin.close()
    fout.close()

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

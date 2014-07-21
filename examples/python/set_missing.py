#
# Copyright 2005-2014 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

import traceback
import sys

from gribapi import *

INPUT='../../data/tigge/tigge_ecmf_pl_t.grib'
OUTPUT='out.grib'
VERBOSE=1 # verbose error reporting

def example():
    fin = open(INPUT)
    fout = open(OUTPUT,'w')

    gid = grib_new_from_file(fin)

    grib_set_long(gid, "scaledValueOfFirstFixedSurface", 15);
    grib_set_long(gid, "scaleFactorOfFirstFixedSurface", 1)
    level=grib_get_double(gid, "level")
    assert( level == 1.5 )

    # set type of level to surface
    grib_set(gid,'typeOfFirstFixedSurface','sfc')
    grib_set_missing(gid,'scaleFactorOfFirstFixedSurface')
    grib_set_missing(gid,'scaledValueOfFirstFixedSurface')

    grib_write(gid,fout)

    grib_release(gid)
    fin.close()
    fout.close()

def main():
    try:
        example()
    except GribInternalError,err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            print >>sys.stderr,err.msg

        return 1

if __name__ == "__main__":
    sys.exit(main())

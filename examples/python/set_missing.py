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

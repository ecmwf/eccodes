import traceback
import sys

from gribapi import *

INPUT='../../data/sample.grib2'
OUTPUT='out.grib'
VERBOSE=1 # verbose error reporting

def example():
    fin = open(INPUT)
    fout = open(OUTPUT,'w')

    gid = grib_new_from_file(fin)

    mgid = grib_multi_new()

    for step in range(12,132,12):
        grib_set(gid,"step",step)
        grib_multi_append(gid,4,mgid)

    grib_multi_write(mgid,fout)

    grib_multi_release(mgid)
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

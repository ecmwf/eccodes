import traceback
import sys

from gribapi import *

INPUT='../../data/reduced_latlon_surface.grib1'
VERBOSE=1 # verbose error reporting

def example():
    f = open(INPUT)

    while 1:
        gid = grib_new_from_file(f)
        if gid is None: break

        iterid = grib_iterator_new(gid,0)

        missingValue = grib_get_double(gid,"missingValue")

        i=0
        while 1:
            result = grib_iterator_next(iterid)
            if not result: break

            [lat,lon,value] = result

            sys.stdout.write("- %d - lat=%.6e lon=%.6e value=" % (i,lat,lon))

            if value == missingValue:
                print "missing"
            else:
                print "%.6f" % value

            i += 1
            
        grib_iterator_delete(iterid)
        grib_release(gid)

    f.close()

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

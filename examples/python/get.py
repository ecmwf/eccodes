import traceback
import sys

from gribapi import *

INPUT='../../data/reduced_latlon_surface.grib1'
VERBOSE=1 # verbose error reporting

def example():
    f = open(INPUT)

    keys = [
        'Ni',
        'Nj',
        'latitudeOfFirstGridPointInDegrees',
        'longitudeOfFirstGridPointInDegrees',
        'latitudeOfLastGridPointInDegrees',
        'longitudeOfLastGridPointInDegrees',
        ]

    while 1:
        gid = grib_new_from_file(f)
        if gid is None: break

        for key in keys:
            if not grib_is_defined(gid,key): raise Exception("Key was not defined")
            print '%s=%s' % (key,grib_get(gid,key))

        if grib_is_defined(gid,"A_very_silly_girl"): raise Exception("Key was defined")

        print 'There are %d values, average is %f, min is %f, max is %f' % (
                  grib_get_size(gid,'values'),
                  grib_get(gid,'average'),
                  grib_get(gid,'min'),
                  grib_get(gid,'max')
               )

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

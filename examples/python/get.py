import traceback
import sys

# Copyright 2005-2012 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

from eccode import *

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
            if not is_defined(gid,key): raise Exception("Key was not defined")
            print '%s=%s' % (key,get(gid,key))

        if is_defined(gid,"A_very_silly_girl"): raise Exception("Key was defined")
        
        print 'There are %d values, average is %f, min is %f, max is %f' % (
                  get_size(gid,'values'),
                  get(gid,'average'),
                  get(gid,'min'),
                  get(gid,'max')
               )

        release(gid)

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

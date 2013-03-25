#!/usr/bin/env python

import sys
from gribapi import *
import profile

def main():
    infile = sys.argv[1]
    fid = open(infile,"r")
    count = grib_count_in_file(fid)

    for i in range(count):
        gid = grib_new_from_file(fid)
        nval = grib_get_size(gid,"values")
        iterid = grib_iterator_new(gid,0)

        missingValue = grib_get_double(gid,"missingValue")

        i=0
        while 1:
            result = grib_iterator_next(iterid)
            if not result: break

            [lat,lon,value] = result

            sys.stdout.write("- %d - lat=%.6f lon=%.6f value=" % (i,lat,lon))

            if value == missingValue:
                print "missing"
            else:
                print "%.6f" % value

            i += 1
            
        grib_iterator_delete(iterid)
        grib_release(gid)

    fid.close()

if __name__ == "__main__":
    #profile.run('main()')
    main()

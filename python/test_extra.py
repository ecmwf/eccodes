#!/usr/bin/env python

from gribapi import *
import sys

input=sys.argv[1]

fid = open(input,"r")
oid = open("test","w")

while 1:
    gid = grib_new_from_file(fid)
    if gid is None: break

    print grib_get(gid,"centre")
    print grib_get(gid,"centre",int)
    print grib_get(gid,"centre",float)
    print grib_get(gid,"centre",str)

    print "Nvalues = ",grib_get_size(gid,"values")

    values = grib_get_array(gid,"values")
    print values[:10]
    values = values * 2

    grib_set_double_array(gid,"values",values)

    values = grib_get_array(gid,"values")
    print values[:10]
    values = values / 2

    grib_set_double_array(gid,"values",[1])

    values = grib_get_values(gid)
    print values[:10]

    print grib_get(gid,"bitsPerValue")
    grib_set(gid,"bitsPerValue",10)
    print grib_get(gid,"bitsPerValue")

    grib_write(gid,oid)

    grib_release(gid)

oid.close()
fid.close()

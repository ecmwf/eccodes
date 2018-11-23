#!/usr/bin/env python

import sys
from gribapi import *

infile = sys.argv[1]
fid = open(infile,"r")
count = grib_count_in_file(fid)

for i in range(count):
    gid = grib_new_from_file(fid)
    iterid = grib_keys_iterator_new(gid)

#    grib_skip_computed(iterid)
#    grib_skip_coded(iterid)
#    grib_skip_edition_specific(iterid)
#    grib_skip_duplicated(iterid)
#    grib_skip_read_only(iterid)
#    grib_skip_function(iterid)

    while grib_keys_iterator_next(iterid):
        keyname = grib_keys_iterator_get_name(iterid)
        keyval = grib_get_string(iterid,keyname)
        keytype = grib_get_native_type(gid,keyname)
        print "%s = %s (%s)" % (keyname,keyval,str(keytype))

    grib_keys_iterator_delete(iterid)
    grib_release(gid)

fid.close()



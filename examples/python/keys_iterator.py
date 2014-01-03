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

INPUT='../../data/reduced_latlon_surface.grib1'
VERBOSE=1 # verbose error reporting

def example():
    f = open(INPUT)

    while 1:
        gid = grib_new_from_file(f)
        if gid is None: break

        iterid = grib_keys_iterator_new(gid,'ls')

        # Different types of keys can be skipped
        # grib_skip_computed(iterid)
        # grib_skip_coded(iterid)
        # grib_skip_edition_specific(iterid)
        # grib_skip_duplicates(iterid)
        # grib_skip_read_only(iterid)
        # grib_skip_function(iterid)

        while grib_keys_iterator_next(iterid):
            keyname = grib_keys_iterator_get_name(iterid)
            keyval = grib_get_string(iterid,keyname)
            print "%s = %s" % (keyname,keyval)

        grib_keys_iterator_delete(iterid)
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

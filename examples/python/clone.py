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
import random

from gribapi import *

INPUT='../../data/constant_field.grib1'
OUTPUT='out.grib'
VERBOSE=1 # verbose error reporting

def example():
    fin = open(INPUT)
    fout = open(OUTPUT,'w')

    gid = grib_new_from_file(fin)

    nx = grib_get(gid,'Ni')
    ny = grib_get(gid,'Nj')

    for step in range(0,24,6):
        clone_id = grib_clone(gid)
        grib_set(clone_id,'step',step)

        values = [random.random() for i in range(nx*ny)]

        grib_set_values(clone_id,values)

        grib_write(clone_id,fout)
        grib_release(clone_id)

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

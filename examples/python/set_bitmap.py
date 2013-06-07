import traceback
import sys

# Copyright 2005-2013 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

from eccode import *

INPUT   = '../../data/regular_latlon_surface.grib1'
OUTPUT  = 'out.grib'
MISSING = 9999
VERBOSE = 1 # verbose error reporting

def example():
    fin = open(INPUT)
    fout = open(OUTPUT,'w')
    gid = new_from_file(fin)

    set(gid,'missingValue', MISSING)
    values = get_values(gid)
    set(gid, 'bitmapPresent', 1)
    # Change some data values to be missing
    num_missing = 0
    for i in range(100):
        if i % 2 == 0:
            values[i] = MISSING
            num_missing += 1
    set_values(gid, values)
    
    # Check counts of missing and non-missing values
    num_data = get(gid,'numberOfDataPoints',int)
    assert(get(gid,'numberOfCodedValues',int) == num_data-num_missing)
    assert(get(gid,'numberOfMissing',int)     == num_missing)

    write(gid,fout)
    release(gid)
    fin.close()
    fout.close()

def main():
    try:
        example()
    except InternalError,err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            print >> sys.stderr,err.msg

        return 1

if __name__ == "__main__":
    sys.exit(main())


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
from datetime import date

INPUT='../../data/regular_latlon_surface_constant.grib1'
OUTPUT='out.grib'
VERBOSE=1 # verbose error reporting

def example():
    fin = open(INPUT)
    fout = open(OUTPUT,'w')
    gid = grib_new_from_file(fin)

    dt = date.today()
    today = "%d%02d%02d" % (dt.year,dt.month,dt.day)
    set(gid,'dataDate',int(today))
    set(gid,'centre',80)

    centreIntVal = get(gid,'centre',int)
    centreStrVal = get(gid,'centre',str)
    dateStrVal   = get(gid,'dataDate',str)
    assert(centreIntVal == 80)
    assert(centreStrVal == 'cnmc')
    assert(dateStrVal   == today)
    print 'get centre as an integer - centre = %d' % centreIntVal
    print 'get centre as a string - centre = %s' % centreStrVal
    print 'get date as a string - date = %s' % dateStrVal
    
    # Now do the same but using set_key_vals, setting keys all at once
    set_key_vals(gid, 'level=1,centre=98')  # with a String
    assert(get(gid,'centre',str) == 'ecmf')
    assert(get(gid,'level',int)  == 1)
    
    set_key_vals(gid, ['level=2', 'centre=kwbc']) # with a Tuple
    assert(get(gid,'centre',int) == 7)
    assert(get(gid,'level',int)  == 2)
    
    set_key_vals(gid, {'level': 3, 'centre': 84}) # with a Dictionary
    assert(get(gid,'centre',str) == 'lfpw')
    assert(get(gid,'level',int)  == 3)
    
    gts_header(True)
    gts_header(False)

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
            print >>sys.stderr,err.msg

        return 1

if __name__ == "__main__":
    sys.exit(main())

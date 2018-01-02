# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

from __future__ import print_function
import traceback
import sys

from eccodes import *
from datetime import date
from collections import OrderedDict

INPUT = '../../data/regular_latlon_surface_constant.grib1'
OUTPUT = 'out.set.grib'
VERBOSE = 1  # verbose error reporting


def example():
    fin = open(INPUT)
    fout = open(OUTPUT, 'w')
    gid = codes_grib_new_from_file(fin)

    dt = date.today()
    today = "%d%02d%02d" % (dt.year, dt.month, dt.day)
    codes_set(gid, 'dataDate', int(today))
    codes_set(gid, 'centre', 80)

    centreIntVal = codes_get_array(gid, 'centre', int)
    centreStrVal = codes_get_array(gid, 'centre', str)
    dateStrVal = codes_get_array(gid, 'dataDate', str)
    assert (centreIntVal[0] == 80)
    assert (centreStrVal[0] == 'cnmc')
    assert (dateStrVal[0] == today)
    print('get centre as an integer - centre = %d' % centreIntVal[0])
    print('get centre as a string - centre = %s' % centreStrVal[0])
    print('get date as a string - date = %s' % dateStrVal[0])

    # Now do the same but using set_key_vals, setting keys all at once
    print('set keys using one long comma-separated string...')
    codes_set_key_vals(gid, 'level=1,centre=98')
    assert (codes_get(gid, 'centre', str) == 'ecmf')
    assert (codes_get(gid, 'level', int) == 1)

    print('set keys using a list of strings...')
    codes_set_key_vals(gid, ['level=2', 'centre=kwbc'])
    assert (codes_get(gid, 'centre', int) == 7)
    assert (codes_get(gid, 'level', int) == 2)

    print('set keys using a dictionary (order not as specified!)...')
    codes_set_key_vals(gid, {'level': 3, 'centre': 84})
    assert (codes_get(gid, 'centre', str) == 'lfpw')
    assert (codes_get(gid, 'level', int) == 3)

    print('set keys using an ordered dictionary...')
    codes_set_key_vals(gid, OrderedDict([('level', 3), ('centre', 84)]))
    assert (codes_get(gid, 'centre', str) == 'lfpw')
    assert (codes_get(gid, 'level', int) == 3)

    codes_gts_header(True)
    codes_gts_header(False)

    codes_write(gid, fout)
    codes_release(gid)
    fin.close()
    fout.close()


def main():
    try:
        example()
    except CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + '\n')

        return 1


if __name__ == "__main__":
    sys.exit(main())

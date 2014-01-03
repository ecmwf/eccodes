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

INPUT='../../data/tp_ecmwf.grib'
OUTPUT='out.grib'
VERBOSE=1 # verbose error reporting

def example():
    sample_id = grib_new_from_samples("regular_ll_sfc_grib1")
    fin = open(INPUT)
    fout = open(OUTPUT,'w')

    keys = {
        'dataDate':20080104,
        'startStep':0,
        'endStep':12,
        'stepType':'accum',
        'table2Version':2,
        'indicatorOfParameter':61,
        'decimalPrecision':2,
        }

    prev_vals = None
    while 1:
        gid = grib_new_from_file(fin)
        if gid is None: break

        curr_vals = grib_get_values(gid)

        if prev_vals is None:
            result = prev_vals = curr_vals
        else:
            try:
                # works with NumPy
                result = curr_vals - prev_vals
            except TypeError: # but not with array
                result = curr_vals
                for i in range(len(result)):
                    result[i] -= prev_vals[i]

            prev_vals = curr_vals
            keys['startStep'] += 12
            keys['endStep'] += 12

        clone_id = grib_clone(sample_id)

        for key in keys:
            grib_set(clone_id,key,keys[key])

        grib_set_values(clone_id,result * 1000)

        grib_write(clone_id,fout)

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

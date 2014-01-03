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

VERBOSE=1 # verbose error reporting

def example():
    """
    Encoding of the pv coefficients.
    """
    # read the coefficients from file
    pv = []
    for line in open('../../data/60_model_levels'):
        pv.extend([float(x) for x in line.strip().split('\t')])

    numberOfLevels = 60
    numberOfCoefficients = 2 * (numberOfLevels + 1)
    assert(len(pv) == numberOfCoefficients)

    fout = open('out.grib1','w')
    gid = grib_new_from_samples('reduced_gg_sfc_grib1')

    grib_set(gid,'typeOfLevel','hybrid')
    grib_set(gid,'level',2)
    grib_set(gid,'PVPresent',1)
    grib_set_array(gid,'pv',pv)

    grib_write(gid,fout)

    grib_release(gid)
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

if __name__ == '__main__':
    sys.exit(main())

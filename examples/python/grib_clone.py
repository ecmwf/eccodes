#
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

import traceback
import sys
import random

from eccodes import *

INPUT = '../../data/constant_field.grib1'
OUTPUT = 'out.clone.grib'
VERBOSE = 1  # verbose error reporting


def example():
    fin = open(INPUT)
    fout = open(OUTPUT, 'w')

    gid = codes_grib_new_from_file(fin)

    assert codes_is_missing(gid, 'Ni') == False
    assert codes_is_missing(gid, 'Nj') == False
    nx = codes_get(gid, 'Ni')
    ny = codes_get(gid, 'Nj')

    for step in range(0, 24, 6):
        clone_id = codes_clone(gid)
        codes_set(clone_id, 'step', step)

        values = [random.random() for i in range(nx * ny)]

        codes_set_values(clone_id, values)

        codes_write(clone_id, fout)
        codes_release(clone_id)

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

#
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

# Python implementation: get_product_kind
#
# Description: how to process a file containing a mix of messages
#              and print the kind of product (e.g. GRIB, BUFR etc)

from __future__ import print_function
import traceback
import sys
from eccodes import *

VERBOSE = 1  # verbose error reporting


def example():
    if len(sys.argv) < 2:
        print('Usage: ', sys.argv[0], ' file', file=sys.stderr)
        sys.exit(1)

    f = open(sys.argv[1])
    while 1:
        ident = codes_new_from_file(f, CODES_PRODUCT_ANY)
        if ident is None:
            break

        print('product: ', codes_get(ident, 'kindOfProduct', str))

        codes_release(ident)

    f.close()


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

# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

#
# Python implementation: bufr_copy_data
#
# Description: How to copy all the values in the data section that are present in the same
#              position in the data tree and with the same number of values to the output handle.
#              In this example we add the WIGOS sequence 301150 to a SYNOP
#
from __future__ import print_function
import traceback
import sys
from eccodes import *

VERBOSE = 1  # verbose error reporting
WIGOS_SEQ = 301150


def example(input_filename, output_filename):
    f = open(input_filename, 'rb')
    ibufrin = codes_bufr_new_from_file(f)
    f.close()

    codes_set(ibufrin, 'unpack', 1)
    inUE = codes_get_array(ibufrin, 'unexpandedDescriptors').tolist()
    nsubsets = 2

    ibufrout = codes_bufr_new_from_samples('BUFR4')
    # Update the unexpandedDescriptors to add the WIGOS data at the beginning
    outUE = inUE
    outUE.insert(0, WIGOS_SEQ)
    codes_set(ibufrout, 'masterTablesVersionNumber', 28)
    codes_set(ibufrout, 'numberOfSubsets', nsubsets)
    codes_set_array(ibufrout, 'unexpandedDescriptors', outUE)

    # Some dummy WIGOS info
    for i in range(0, nsubsets):
        key = "#{0}#wigosIssuerOfIdentifier".format(i + 1)
        codes_set(ibufrout, key, (i + 1) * 3)

    # Copy across other data in input
    codes_bufr_copy_data(ibufrin, ibufrout)

    outfile = open(output_filename, 'wb')
    codes_write(ibufrout, outfile)
    outfile.close()

    codes_release(ibufrin)
    codes_release(ibufrout)


def main():
    if len(sys.argv) < 3:
        print('Usage: ', sys.argv[0], ' bufr_in bufr_out', file=sys.stderr)
        return 1

    input_filename = sys.argv[1]
    output_filename = sys.argv[2]

    try:
        example(input_filename, output_filename)
    except CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + '\n')

        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())

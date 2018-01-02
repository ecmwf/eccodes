# Copyright 2005-2018 ECMWF.
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
#              position in the data tree and with the same number of values to the output handle
#
from __future__ import print_function
import traceback
import sys
from eccodes import *

VERBOSE = 1  # verbose error reporting


def example(input_filename, output_filename):
    ibufr = codes_new_from_samples('BUFR3', CODES_PRODUCT_BUFR)
    f = open(input_filename)
    ibufrin = codes_bufr_new_from_file(f)
    ivalues = (
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 0)
    codes_set_array(ibufr, 'inputDataPresentIndicator', ivalues)
    codes_set(ibufr, 'edition', 3)
    codes_set(ibufr, 'masterTableNumber', 0)
    codes_set(ibufr, 'bufrHeaderSubCentre', 0)
    codes_set(ibufr, 'bufrHeaderCentre', 98)
    codes_set(ibufr, 'updateSequenceNumber', 1)
    codes_set(ibufr, 'dataCategory', 0)
    codes_set(ibufr, 'dataSubCategory', 140)
    codes_set(ibufr, 'masterTablesVersionNumber', 13)
    codes_set(ibufr, 'localTablesVersionNumber', 1)
    codes_set(ibufr, 'typicalYearOfCentury', 15)
    codes_set(ibufr, 'typicalMonth', 5)
    codes_set(ibufr, 'typicalDay', 4)
    codes_set(ibufr, 'typicalHour', 9)
    codes_set(ibufr, 'typicalMinute', 30)
    codes_set(ibufr, 'numberOfSubsets', 1)
    codes_set(ibufr, 'observedData', 1)
    codes_set(ibufr, 'compressedData', 0)
    ivalues = (
        307011, 7006, 10004, 222000, 101023, 31031, 1031, 1032, 101023, 33007,
        225000, 236000, 101023, 31031, 1031, 1032, 8024, 101001, 225255, 225000,
        236000, 101023, 31031, 1031, 1032, 8024, 101001, 225255,
        1063, 2001, 4001, 4002, 4003, 4004, 4005, 5002,
        6002, 7001, 7006, 11001, 11016, 11017, 11002)
    codes_set_array(ibufr, 'unexpandedDescriptors', ivalues)
    codes_set(ibufrin, 'unpack', 1)
    codes_bufr_copy_data(ibufrin, ibufr)  # Copy data across

    with open(output_filename, 'w') as outfile:
        codes_write(ibufr, outfile)
    codes_release(ibufr)
    codes_release(ibufrin)


def main():
    if len(sys.argv) < 3:
        print('Usage: ', sys.argv[0], ' bufr_in bufr_out', file=sys.stderr)
        sys.exit(1)

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


if __name__ == "__main__":
    sys.exit(main())

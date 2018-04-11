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
#              position in the data tree and with the same number of values to the output handle.
#              This example is for messages which use the operator 203YYY (overridden reference values)
#
from __future__ import print_function
import traceback
import sys
from eccodes import *

VERBOSE = 1  # verbose error reporting

def example(input_filename, output_filename):
    f = open(input_filename)
    ibufrin = codes_bufr_new_from_file(f)
    f.close()

    # Need to unpack to get delayed replications and reference values from input
    codes_set(ibufrin,'unpack',1)
    references=codes_get_array(ibufrin,'inputOverriddenReferenceValues')
    replication=codes_get_array(ibufrin,'delayedDescriptorReplicationFactor')

    ibufrout=codes_clone(ibufrin)

    # Copy over to output
    codes_set_array(ibufrout,'inputOverriddenReferenceValues',references)
    codes_set_array(ibufrout,'inputDelayedDescriptorReplicationFactor',replication)
    # Keep all original descriptors and add 'meanWindDirectionForSurfaceTo1500M' (011044)
    ivalues=( 203014, 7030, 7031, 203255, 307080, 11044, )
    codes_set_array(ibufrout,'unexpandedDescriptors',ivalues)

    codes_bufr_copy_data ( ibufrin,ibufrout)

    outfile=open(output_filename,'wb')
    codes_write(ibufrout,outfile)
    outfile.close()

    codes_release(ibufrin)
    codes_release(ibufrout)


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

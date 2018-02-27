#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh


# Define a common label for all the tmp files
label="bufr_read_scatterometer_f"

# Define tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp

# We check "asca_139.bufr". The path is hardcoded in the example

REDIRECT=/dev/null

# Write the key values into a file
${examples_dir}/eccodes_f_bufr_read_scatterometer 2> $REDIRECT > $fTmp

# Check the results
REFERENCE_FILE=bufr_read_scatterometer_f.ref
diff $REFERENCE_FILE $fTmp

#Clean up
rm -f $fTmp

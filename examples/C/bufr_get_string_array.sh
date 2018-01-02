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
label="bufr_get_string_array_test_c"

# Define tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp

REDIRECT=/dev/null

# Write the output into a file
${examples_dir}/c_bufr_get_string_array 2> $REDIRECT > $fTmp

# Check the results
grep -q 'stationOrSiteName string array size = 128' $fTmp

# Clean up
rm -f $fTmp

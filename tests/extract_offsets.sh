#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

# Define a common label for all the tmp files
label="extract_offsets_test"
temp1="temp.${label}.1"
temp2="temp.${label}.2"
tempLog="temp.${label}.log"

echo "Multi-message BUFR..."
# ---------------------------
input=${data_dir}/bufr/aeolus_wmo_26.bufr
$EXEC ${test_dir}/extract_offsets  $input > $temp1
${tools_dir}/bufr_get -p offset:i  $input > $temp2
diff $temp1 $temp2

echo "Multi-message GRIBs..."
# --------------------------
inputs="${data_dir}/mixed.grib ${data_dir}/test.grib1  ${data_dir}/v.grib2"
for input in $inputs; do
    $EXEC ${test_dir}/extract_offsets  $input > $temp1
    ${tools_dir}/grib_get -p offset:i  $input > $temp2
    diff $temp1 $temp2
done

echo "Test with invalid inputs..."
# ---------------------------------
set +e
$EXEC ${test_dir}/extract_offsets ${data_dir} > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "is a directory" $tempLog

set +e
$EXEC ${test_dir}/extract_offsets ${data_dir}/bad.grib > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Wrong message length" $tempLog

# Clean up
rm -f $temp1 $temp2 $tempLog

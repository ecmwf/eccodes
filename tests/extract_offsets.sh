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
tempRef="temp.${label}.ref"

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

echo "Multi-message BUFR..."
# ---------------------------
input=${data_dir}/bufr/aeolus_wmo_26.bufr
$EXEC ${test_dir}/extract_offsets -o $input > $temp1
${tools_dir}/bufr_get -p offset:i  $input > $temp2
diff $temp1 $temp2

$EXEC ${test_dir}/extract_offsets -s $input > $temp1
${tools_dir}/bufr_get -p totalLength  $input > $temp2
diff $temp1 $temp2


echo "Multi-message GRIBs..."
# --------------------------
inputs="${data_dir}/mixed.grib ${data_dir}/test.grib1  ${data_dir}/v.grib2"
for input in $inputs; do
    $EXEC ${test_dir}/extract_offsets -o $input > $temp1
    ${tools_dir}/grib_get -p offset:i  $input > $temp2
    diff $temp1 $temp2

    $EXEC ${test_dir}/extract_offsets -s $input > $temp1
    ${tools_dir}/grib_get -p totalLength  $input > $temp2
    diff $temp1 $temp2
done

echo "GTS headers and padding..."
# -------------------------------
input=${data_dir}/gts.grib
$EXEC ${test_dir}/extract_offsets -o -s $input > $temp1
cat > $tempRef << EOF
41
170
299
428
84
84
84
84
EOF
diff $tempRef $temp1

echo "Test with invalid inputs..."
# ---------------------------------
set +e
$EXEC ${test_dir}/extract_offsets -o ${data_dir} > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "is a directory" $tempLog

set +e
$EXEC ${test_dir}/extract_offsets -o ${data_dir}/bad.grib > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Wrong message length" $tempLog

set +e
$EXEC ${test_dir}/extract_offsets -o nonexistentfile > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Unable to read file" $tempLog


# Clean up
rm -f $temp1 $temp2 $tempLog $tempRef

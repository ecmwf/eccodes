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

label="bufr_check_descriptors_test"
tempInput=temp.$label.table
tempText=temp.$label.txt

for file in `find ${ECCODES_DEFINITION_PATH}/bufr/ -name 'element.table' -print`
do
  ${test_dir}/bufr_check_descriptors $file
done

# Check we detect bad inputs
# ---------------------------
echo "abc" > $tempInput
set +e
${test_dir}/bufr_check_descriptors $tempInput >$tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Number of columns .* required miniumum" $tempText


echo "a|b|c|d|e|f|g|h" > $tempInput
set +e
${test_dir}/bufr_check_descriptors $tempInput >$tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "descriptor code .* is not numeric" $tempText


echo "1234567|b|c|d|e|f|g|h" > $tempInput
set +e
${test_dir}/bufr_check_descriptors $tempInput >$tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "descriptor code .* is not 6 digits" $tempText


echo "010192|depthOfRoots|rubbish|DEPTH OF ROOTS|m|2|0|10" > $tempInput
set +e
${test_dir}/bufr_check_descriptors $tempInput >$tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "descriptor key type .* is not valid" $tempText
rm -f $tempText

echo "010192|depthOfRoots|double|DEPTH OF ROOTS|m|x|0|0" > $tempInput
set +e
${test_dir}/bufr_check_descriptors $tempInput >$tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "descriptor scale .* is not numeric" $tempText


echo "010192|depthOfRoots|double|DEPTH OF ROOTS|m|0|a|0" > $tempInput
set +e
${test_dir}/bufr_check_descriptors $tempInput >$tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "descriptor reference .* is not numeric" $tempText


echo "010192|depthOfRoots|double|DEPTH OF ROOTS|m|0|0|p" > $tempInput
set +e
${test_dir}/bufr_check_descriptors $tempInput >$tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
cat $tempText
grep -q "descriptor width" $tempText


# Clean up
rm -f $tempInput $tempText

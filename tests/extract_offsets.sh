#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

# Define a common label for all the tmp files
label="bufr_extract_offsets_test"
temp1="temp.${label}.1"
temp2="temp.${label}.2"

echo "Multi-message BUFR..."
# ---------------------------
input=${data_dir}/bufr/aeolus_wmo_26.bufr
$EXEC ${test_dir}/extract_offsets  $input > $temp1
${tools_dir}/bufr_get -p offset:i  $input    > $temp2
diff $temp1 $temp2

echo "Multi-message GRIB..."
# --------------------------
input=${data_dir}/mixed.grib
$EXEC ${test_dir}/extract_offsets  $input > $temp1
${tools_dir}/grib_get -p offset:i  $input > $temp2
diff $temp1 $temp2


echo "Test with invalid inputs..."
# ---------------------------------
set +e
$EXEC ${test_dir}/extract_offsets ${data_dir}
status=$?
set -e
[ $status -ne 0 ]

rm -f $temp1 $temp2

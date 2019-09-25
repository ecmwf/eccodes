#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

# Define a common label for all the tmp files
label="bufr_extract_headers_test"

temp1="temp.${label}.1"
temp2="temp.${label}.2"

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`

KEYS='edition,totalLength,bufrHeaderCentre,dataCategory,masterTablesVersionNumber,typicalMonth,typicalDay,rdbType,localYear,qualityControl,numberOfSubsets,compressedData,ident'

for bf in ${bufr_files}; do
    input=${data_dir}/bufr/$bf
    $EXEC ${test_dir}/bufr_extract_headers $KEYS $input > $temp1
    ${tools_dir}/bufr_get            -f -p $KEYS $input > $temp2
    diff -w $temp1 $temp2
done


rm -f $temp1 $temp2

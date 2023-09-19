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

label="grib_count_test"

grib_files=`cat ${data_dir}/grib_data_files.txt`
for file in ${grib_files}; do
  if [ "$file" = "bad.grib" -o "$file" = "budg" ]; then continue; fi
  input=${data_dir}/$file
  ${tools_dir}/grib_count $input
done

# Specific files
count=`${tools_dir}/grib_count ${data_dir}/test.grib1`
[ $count -eq 42 ]

count=`${tools_dir}/grib_count ${data_dir}/mixed.grib`
[ $count -eq 14 ]

count=`${tools_dir}/grib_count ${data_dir}/tigge_ecmwf.grib2`
[ $count -eq 248 ]

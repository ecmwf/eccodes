#!/bin/sh
# Copyright 2005-2017 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

#Define a common label for all the tmp files
label="codes_split_file"

# Do all the work in a temporary directory
temp_dir=tempdir.${label}
mkdir -p $temp_dir

# Test 1: File with 3 messages
# -----------------------------
cp ${data_dir}/mixed.grib $temp_dir
input=$temp_dir/mixed.grib
${tools_dir}/codes_split_file 3 $input
# There should now be 3 new files. Make sure they are valid
${tools_dir}/grib_ls $temp_dir/mixed.grib_1
${tools_dir}/grib_ls $temp_dir/mixed.grib_2
${tools_dir}/grib_ls $temp_dir/mixed.grib_3

total=`${tools_dir}/codes_count $temp_dir/mixed.grib_[1-3]`
[ $total -eq 14 ]

# Test 2: File with 248 messages
# -----------------------------
cp ${data_dir}/tigge_ecmwf.grib2 $temp_dir
input=$temp_dir/tigge_ecmwf.grib2
${tools_dir}/codes_split_file 10 $input
total=`${tools_dir}/codes_count $temp_dir/tigge_ecmwf.grib2_[0-9]*`
[ $total -eq 248 ]


# Clean up
rm -fr $temp_dir

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
${tools_dir}/grib_ls $temp_dir/mixed.grib_01
${tools_dir}/grib_ls $temp_dir/mixed.grib_02
${tools_dir}/grib_ls $temp_dir/mixed.grib_03

total=`${tools_dir}/codes_count $temp_dir/mixed.grib_*`
[ $total -eq 14 ]

cat $temp_dir/mixed.grib_* > temp
${tools_dir}/grib_compare $input temp


# Test 2: File with 248 messages
# -----------------------------
cp ${data_dir}/tigge_ecmwf.grib2 $temp_dir
input=$temp_dir/tigge_ecmwf.grib2
${tools_dir}/codes_split_file 10 $input
total=`${tools_dir}/codes_count $temp_dir/tigge_ecmwf.grib2_[0-9]*`
[ $total -eq 248 ]

cat $temp_dir/tigge_ecmwf.grib2_0[1-9] $temp_dir/tigge_ecmwf.grib2_10 > temp
${tools_dir}/grib_compare $input temp


# Clean up
rm -fr $temp_dir

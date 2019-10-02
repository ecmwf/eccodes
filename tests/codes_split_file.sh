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
pwd
#Define a common label for all the tmp files
label="codes_split_file"
temp=$label.temp.grib

# Do all the work in a temporary directory
temp_dir=tempdir.${label}
mkdir -p $temp_dir
cd $temp_dir

# Test 1: File with 3 messages
# -----------------------------
cp ${data_dir}/mixed.grib ./
input=mixed.grib
${tools_dir}/codes_split_file 3 $input
# There should now be 3 new files. Make sure they are valid
${tools_dir}/grib_ls mixed.grib_001
${tools_dir}/grib_ls mixed.grib_002
${tools_dir}/grib_ls mixed.grib_003

total=`${tools_dir}/codes_count mixed.grib_*`
[ $total -eq 14 ]

cat mixed.grib_* > $temp
${tools_dir}/grib_compare $input $temp


# Test 2: File with 248 messages
# -----------------------------
cp ${data_dir}/tigge_ecmwf.grib2 ./
input=tigge_ecmwf.grib2
${tools_dir}/codes_split_file 10 $input
total=`${tools_dir}/codes_count tigge_ecmwf.grib2_[0-9]*`
[ $total -eq 248 ]

cat tigge_ecmwf.grib2_[0-9][0-9][0-9] > $temp
${tools_dir}/grib_compare $input $temp


# Test 3: nchunk=-1 File with 248 messages
# -----------------------------
cp ${data_dir}/tigge_ecmwf.grib2 ./
input=tigge_ecmwf.grib2
${tools_dir}/codes_split_file -1 $input
total=`${tools_dir}/codes_count tigge_ecmwf.grib2_[0-9]*`
[ $total -eq 248 ]

cat tigge_ecmwf.grib2_[0-9][0-9][0-9] > $temp
${tools_dir}/grib_compare $input $temp


# Clean up
cd $test_dir
rm -fr $temp_dir

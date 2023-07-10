#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label="grib_check_gaussian_grids_test"
tempGrib=temp.$label.grib
tempText=temp.$label.txt


# Check all sample GRIBs with a Gaussian grid
samples_dir=$ECCODES_SAMPLES_PATH
for gg in ${samples_dir}/reduced_gg_* ${samples_dir}/regular_gg_*; do
   ${tools_dir}/grib_check_gaussian_grid $gg >/dev/null
   grib_check_key_equals $gg "global" 1
done

for gg in ${samples_dir}/reduced_gg_*tmpl; do
   ${tools_dir}/grib_set -s global=1 $gg $tempGrib
   ${tools_dir}/grib_check_gaussian_grid $tempGrib
done


# Set wrong angle and re-test. Should fail
input=$samples_dir/reduced_gg_pl_1280_grib2.tmpl
${tools_dir}/grib_set -s longitudeOfLastGridPoint=359929680 $input $tempGrib
set +e
${tools_dir}/grib_check_gaussian_grid -v $tempGrib 2> $tempText
status=$?
set -e
[ $status -eq 1 ]
grep -q "Error: longitudeOfLastGridPointInDegrees.*but should be" $tempText


# Set wrong numberOfDataPoints and re-test. Should fail
input=$samples_dir/reduced_gg_pl_96_grib2.tmpl
${tools_dir}/grib_set -s numberOfDataPoints=44 $input $tempGrib
set +e
${tools_dir}/grib_check_gaussian_grid -v $tempGrib 2> $tempText
status=$?
set -e
[ $status -eq 1 ]
grep -q "Error: Sum of pl array 50662 does not match numberOfDataPoints 44" $tempText


# Set wrong numberOfValues and re-test. Should fail
input=$samples_dir/reduced_gg_pl_96_grib2.tmpl
${tools_dir}/grib_set -s numberOfValues=44 $input $tempGrib
set +e
${tools_dir}/grib_check_gaussian_grid -v $tempGrib 2> $tempText
status=$?
set -e
[ $status -eq 1 ]
cat $tempText
grep -q "Error: Sum of pl array 50662 does not match numberOfValues 44" $tempText


rm -f $tempGrib $tempText

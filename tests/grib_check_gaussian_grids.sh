#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

label="grib_check_gaussian_grids"
temp=temp.$label.grib


# Check all sample GRIBs with a Gaussian grid
samples_dir=$ECCODES_SAMPLES_PATH
for gg in ${samples_dir}/reduced_gg_* ${samples_dir}/regular_gg_*; do
   ${tools_dir}/grib_check_gaussian_grid $gg >/dev/null
   grib_check_key_equals $gg "global" 1
done

# Set wrong angle and re-test. Should fail
input=$samples_dir/reduced_gg_pl_1280_grib2.tmpl
${tools_dir}/grib_set -s longitudeOfLastGridPoint=359929680 $input $temp
set +e
${tools_dir}/grib_check_gaussian_grid -v $temp
status=$?
set -e
[ $status -eq 1 ]


rm -f $temp

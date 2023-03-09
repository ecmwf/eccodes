#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label="grib_global_test"

# Check Gaussian grids
files_global="
    reduced_gaussian_lsm.grib1
    reduced_gaussian_model_level.grib1
    reduced_gaussian_pressure_level.grib1
    reduced_gaussian_pressure_level_constant.grib1
    reduced_gaussian_surface.grib1
    reference_ensemble_mean.grib1
    reference_stdev.grib1
    regular_gaussian_model_level.grib1
    regular_gaussian_pressure_level.grib1
    regular_gaussian_pressure_level_constant.grib1
    regular_gaussian_surface.grib1
    reduced_gaussian_model_level.grib2
    reduced_gaussian_pressure_level.grib2
    reduced_gaussian_pressure_level_constant.grib2
    reduced_gaussian_surface.grib2
    reduced_gaussian_surface_jpeg.grib2
    regular_gaussian_model_level.grib2
    regular_gaussian_pressure_level.grib2
    regular_gaussian_pressure_level_constant.grib2
    regular_gaussian_surface.grib2
    tigge/tigge_ecmf_pl_gh.grib
    tigge/tigge_sbsj_pl_q.grib
"

files_subarea="
    reduced_gaussian_sub_area.grib1
    reduced_gaussian_sub_area.grib2
"

for gg in $files_global; do
    filepath=${data_dir}/$gg
    grib_check_key_equals $filepath "global" 1
done

for gg in $files_subarea; do
    filepath=${data_dir}/$gg
    grib_check_key_equals $filepath "global" 0
done


# ECC-1542: Invalid pl array in a Gaussian grid
# -----------------------------------------------
sample1=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib1.tmpl
sample2=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
tempGrib=temp.$label.grib
tempText=temp.$label.txt
tempFilt=temp.$label.filt

# Insert a zero into the pl array
cat > $tempFilt <<EOF
 meta pli element(pl, 3);
 set pli = 0;
 write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample2
set +e
${tools_dir}/grib_get -p numberOfDataPointsExpected $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid pl array: entry at index=3 is zero" $tempText

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample1
set +e
${tools_dir}/grib_get -l 0,0 $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid pl array: entry at index=3 is zero" $tempText


# Clean up
rm -f $tempGrib $tempText $tempFilt

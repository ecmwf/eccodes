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

REDIRECT=/dev/null

label="grib_dump_test"
temp=temp.$label.txt

if [ $HAVE_MEMFS -eq 1 ]; then
    unset ECCODES_DEFINITION_PATH
    unset ECCODES_SAMPLES_PATH
fi

files="
constant_field.grib1
lfpw.grib1
missing_field.grib1
reduced_gaussian_lsm.grib1
reduced_gaussian_model_level.grib1
reduced_gaussian_pressure_level.grib1
reduced_gaussian_pressure_level_constant.grib1
reduced_gaussian_sub_area.grib1
reduced_gaussian_surface.grib1
reduced_latlon_surface.grib1
reduced_latlon_surface_constant.grib1
reference_ensemble_mean.grib1
reference_stdev.grib1
regular_gaussian_model_level.grib1
regular_gaussian_pressure_level.grib1
regular_gaussian_pressure_level_constant.grib1
regular_gaussian_surface.grib1
regular_latlon_surface.grib1
regular_latlon_surface_constant.grib1
second_ord_rbr.grib1
small_ensemble.grib1
spectral_complex.grib1
spherical_model_level.grib1
spherical_pressure_level.grib1
constant_field.grib2
gfs.c255.grib2
jpeg.grib2
missing.grib2
multi.grib2
multi_created.grib2
reduced_gaussian_model_level.grib2
reduced_gaussian_pressure_level.grib2
reduced_gaussian_pressure_level_constant.grib2
reduced_gaussian_sub_area.grib2
reduced_gaussian_surface.grib2
reduced_gaussian_surface_jpeg.grib2
reduced_latlon_surface.grib2
reduced_latlon_surface_constant.grib2
regular_gaussian_model_level.grib2
regular_gaussian_pressure_level.grib2
regular_gaussian_pressure_level_constant.grib2
regular_gaussian_surface.grib2
regular_latlon_surface.grib2
regular_latlon_surface_constant.grib2
sample.grib2
spherical_model_level.grib2
spherical_pressure_level.grib2
test_uuid.grib2
tigge_af_ecmwf.grib2
tigge_cf_ecmwf.grib2
v.grib2
"

for file in $files; do
   if [ -f ${data_dir}/$file ]; then
      ${tools_dir}/grib_dump -O ${data_dir}/$file 2> $REDIRECT > $REDIRECT
   fi
done


# Test for dumping a section
if [ $HAVE_JPEG -eq 0 ]; then
    # No JPEG decoding enabled so dumping section 7 will issue errors
    # but dumping non-data sections should work
    file=${data_dir}/jpeg.grib2
    ${tools_dir}/grib_dump -O -p section_3,section_4 $file > $temp 2>&1
    set +e
    # Look for the word ERROR in output. We should not find any
    grep -q 'ERROR ' $temp
    if [ $? -eq 0 ]; then
        echo "grib_dump on $file: found string ERROR in grib_dump output!"
        cat $temp
        exit 1
    fi
    set -e
fi

rm -f $temp

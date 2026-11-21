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

# Reading any numeric GRIB key as float agrees with reading it as double

label="codes_get_float_test"
temp=temp.$label.grib

for s in GRIB1 GRIB2 regular_ll_sfc_grib1 regular_ll_sfc_grib2 reduced_gg_pl_32_grib1 reduced_gg_pl_32_grib2 sh_ml_grib1 sh_ml_grib2; do
    $EXEC ${test_dir}/codes_get_float $ECCODES_SAMPLES_PATH/$s.tmpl
done

files="regular_latlon_surface.grib1 regular_latlon_surface.grib2 reduced_gaussian_model_level.grib1
       reduced_gaussian_model_level.grib2 spherical_pressure_level.grib1 spherical_pressure_level.grib2
       missing_field.grib1 missing.grib2 constant_field.grib2 simple_bitmap.grib lfpw.grib1 gen_ext.grib
       gfs.complex.mvmu.grib2 grid_ieee.grib tigge_pf_ecmwf.grib2 satellite.grib mercator.grib2"
if [ $HAVE_AEC -eq 1 ]; then files="$files ccsds.grib2"; fi
if [ $HAVE_JPEG -eq 1 ]; then files="$files jpeg.grib2"; fi
for f in $files; do
    $EXEC ${test_dir}/codes_get_float ${data_dir}/$f
done

# spectral_simple
${tools_dir}/grib_set -r -s packingType=spectral_simple ${data_dir}/spherical_pressure_level.grib2 $temp
$EXEC ${test_dir}/codes_get_float $temp

rm -f $temp

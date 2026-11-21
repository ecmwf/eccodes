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

# codes_set_float_array must behave like codes_set_double_array for every key and packing type

label="codes_set_float_array_test"
temp=temp.$label.grib

files=""
for s in GRIB1 GRIB2 regular_ll_sfc_grib1 regular_ll_sfc_grib2 reduced_gg_pl_32_grib2 sh_ml_grib1 sh_ml_grib2; do
    files="$files $ECCODES_SAMPLES_PATH/$s.tmpl"
done
for f in $files; do
    $EXEC ${test_dir}/codes_set_float_array $f
done

# Other packing types
input=${data_dir}/regular_latlon_surface.grib2
packings="grid_simple grid_ieee grid_second_order grid_complex grid_complex_spatial_differencing"
if [ $HAVE_AEC -eq 1 ]; then packings="$packings grid_ccsds"; fi
if [ $HAVE_JPEG -eq 1 ]; then packings="$packings grid_jpeg"; fi
for p in $packings; do
    ${tools_dir}/grib_set -r -s packingType=$p $input $temp
    grib_check_key_equals $temp packingType $p
    $EXEC ${test_dir}/codes_set_float_array $temp
done
input=${data_dir}/regular_latlon_surface.grib1
for p in grid_simple grid_second_order grid_ieee; do
    ${tools_dir}/grib_set -r -s packingType=$p $input $temp
    $EXEC ${test_dir}/codes_set_float_array $temp
done

rm -f $temp

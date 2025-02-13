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

REDIRECT=/dev/null
label="grib_isGridded_isSpectral_test"
tempGrib=temp.$label.grib
tempText=temp.$label.txt
rm -f $tempGrib $tempText

# Check the keys isSpectral and isGridded
# for specific grib types
# ----------------------------------------
grib1_gridded_types="
  regular_ll
  reduced_ll
  mercator
  lambert
  polar_stereographic
  albers
  rotated_ll
  stretched_ll
  stretched_rotated_ll
  regular_gg
  rotated_gg
  stretched_gg
  stretched_rotated_gg
  reduced_gg
  reduced_rotated_gg
  reduced_stretched_gg
  reduced_stretched_rotated_gg
  regular_rotated_gg
  regular_stretched_gg
  regular_stretched_rotated_gg
  space_view"
grib1_spectral_types="
  sh
  rotated_sh
  stretched_sh
  stretched_rotated_sh"

grib2_gridded_types="
  regular_ll
  reduced_ll
  rotated_ll
  stretched_ll
  stretched_rotated_ll
  mercator
  transverse_mercator
  polar_stereographic
  lambert
  albers
  regular_gg
  reduced_gg
  rotated_gg
  reduced_rotated_gg
  stretched_gg
  reduced_stretched_gg
  stretched_rotated_gg
  reduced_stretched_rotated_gg
  regular_rotated_gg
  regular_stretched_gg
  regular_stretched_rotated_gg
  space_view
  triangular_grid
  unstructured_grid
  equatorial_azimuthal_equidistant
  azimuth_range
  irregular_latlon
  lambert_azimuthal_equal_area
  lambert_lam
  mercator_lam
  polar_stereographic_lam
  healpix
"
grib2_spectral_types="
  sh
  rotated_sh
  stretched_sh
  stretched_rotated_sh
  lambert_bf
  mercator_bf
  polar_stereographic_bf
"

sample1_gridded=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample1_spectral=$ECCODES_SAMPLES_PATH/sh_ml_grib1.tmpl
for gt in $grib1_gridded_types; do
    ${tools_dir}/grib_set -s gridType=$gt $sample1_gridded $tempGrib
    grib_check_key_equals $tempGrib 'isGridded,isSpectral' '1 0'
done
for gt in $grib1_spectral_types; do
    ${tools_dir}/grib_set -s gridType=$gt $sample1_spectral $tempGrib
    grib_check_key_equals $tempGrib 'isGridded,isSpectral' '0 1'
    grib_check_key_equals $tempGrib gridType $gt
done

sample2_gridded=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
sample2_spectral=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl
for gt in $grib2_gridded_types; do
    ${tools_dir}/grib_set -s gridType=$gt $sample2_gridded $tempGrib
    grib_check_key_equals $tempGrib 'isGridded,isSpectral' '1 0'
done
for gt in $grib2_spectral_types; do
    ${tools_dir}/grib_set -s gridType=$gt $sample2_spectral $tempGrib
    grib_check_key_equals $tempGrib 'isGridded,isSpectral' '0 1'
    grib_check_key_equals $tempGrib gridType $gt
done

grib_check_key_equals $data_dir/reduced_latlon_surface.grib1 'isGridded,isSpectral' '1 0'
grib_check_key_equals $data_dir/reduced_latlon_surface.grib2 'isGridded,isSpectral' '1 0'
grib_check_key_equals $data_dir/mercator.grib2 'isGridded,isSpectral' '1 0'


# Check every entry in the latest Code Table 3.1
# -----------------------------------------------
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
latestOfficial=$( ${tools_dir}/grib_get -p tablesVersionLatestOfficial $sample2 )
latest_codetable_file=$ECCODES_DEFINITION_PATH/grib2/tables/$latestOfficial/3.1.table
if [ -f "$latest_codetable_file" ]; then
  gtypes=$( awk '$1 !~ /#/ && $1 < 65000 {print $1}' $latest_codetable_file )
  for gt in $gtypes; do
    # echo "Doing $gt "
    ${tools_dir}/grib_set -s tablesVersion=$latestOfficial,gridDefinitionTemplateNumber=$gt $sample2 $tempGrib
    ${tools_dir}/grib_get -p isGridded,gridDefinitionDescription $tempGrib
  done
fi

# Clean up
rm -f $tempGrib $tempText

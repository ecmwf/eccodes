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

if [ $HAVE_GEOGRAPHY -eq 0 ]; then
    exit 0
fi

label="grib_iterator_test"
tempText=temp.$label.txt
tempGrib=temp.$label.grib

files="reduced_latlon_surface.grib1 \
      reduced_gaussian_pressure_level.grib1 \
      regular_gaussian_pressure_level.grib1 \
      reduced_gaussian_sub_area.grib1 \
      regular_latlon_surface.grib1 \
      reduced_latlon_surface.grib2  \
      reduced_gaussian_pressure_level.grib2 \
      regular_gaussian_pressure_level.grib2 \
      reduced_gaussian_sub_area.grib2 \
      regular_latlon_surface.grib2"

for f in $files; do
  file=${data_dir}/$f
  # Must exclude the first line of grib_get_data which is "Latitude Longitude Value"
  iterator_count=`${tools_dir}/grib_get_data -m 9999:missing -f -p centre -F "%g" -w count=1 $file | grep -v Lat |wc -l `
  numberOfPoints=`${tools_dir}/grib_get -w count=1 -p numberOfPoints $file`
  [ $numberOfPoints = ${iterator_count} ]
done


# ECC-822: Increase lat/lon decimals using default grib_get_data
${tools_dir}/grib_get_data -L%12.6f%11.5f ${data_dir}/regular_latlon_surface.grib2 > $tempText

${tools_dir}/grib_get_data -p shortName,level ${data_dir}/regular_latlon_surface.grib2 > $tempText
grep -q "Latitude Longitude Value shortName level" $tempText


# Run on a spectral field - should just print out its values
${tools_dir}/grib_get_data "$samp_dir/sh_ml_grib2.tmpl" > $tempText


# ECC-1781: Reduced Gaussian grid with rotation
# ---------------------------------------------
${tools_dir}/grib_get_data $samp_dir/reduced_rotated_gg_pl_320_grib1.tmpl > $tempText
${tools_dir}/grib_get_data $samp_dir/reduced_rotated_gg_pl_320_grib2.tmpl > $tempText
ECCODES_DEBUG=-1 ${tools_dir}/grib_get_data ${data_dir}/reduced_gaussian_sub_area.grib2 > $tempText 2>&1
if grep -q "sub-area num points=53564" "$tempText" || grib_check_key_equals "${data_dir}/reduced_gaussian_sub_area.grib2" numberOfDataPoints 53564
then
    : # success
else
    exit 42
fi

# -w option
input=$data_dir/tigge_cf_ecmwf.grib2
${tools_dir}/grib_get_data -w count=11 $input > $tempText

# Skip missing
input=$data_dir/simple_bitmap.grib
grib_check_key_equals $input bitmapPresent 1
${tools_dir}/grib_get_data $input > $tempText

# Print a double key
input=$data_dir/simple_bitmap.grib
${tools_dir}/grib_get_data -p referenceValue $input > $tempText

# Print a key with missing value
input=$data_dir/sample.grib2
${tools_dir}/grib_get_data -p scaleFactorOfEarthMajorAxis $input > $tempText

# Legacy Gaussian sub-area (produced by old ProdGen)
# See ECC-906:
#   grib_get_data not working correctly with old-style sub-areas of reduced grids
# ------------------------------------------------------------------------------
input=$data_dir/reduced_gaussian_sub_area.legacy.grib1
if [ -f "$input" ]; then
  ${tools_dir}/grib_get_data $input > $tempText
  grib_check_key_equals $input legacyGaussSubarea 1

  ECCODES_DEBUG=-1 ${tools_dir}/grib_ls -p numberOfDataPoints $input > $tempText 2>&1
  grep -q "LEGACY MODE activated. Count.=253982. changed to num values.=254139" $tempText
fi

# Iterate with DEBUG on
input=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
ECCODES_DEBUG=1 ${tools_dir}/grib_get_data $input > $tempText 2>&1
if grep -q "global num points=6114" "$tempText" || grib_check_key_equals "$input" numberOfDataPoints 6114
then
    : # success
else
    exit 42
fi


# Clean up
rm -f $tempText $tempGrib

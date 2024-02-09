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

label="grib_png_test"
temp=${label}".grib.tmp"
temp1=${label}".1.tmp"
temp2=${label}".2.tmp"
tempErr=${label}.err

files="
 reduced_gaussian_model_level.grib2
 reduced_gaussian_sub_area.grib2
 regular_gaussian_model_level.grib2
 regular_latlon_surface_constant.grib2
 test_file.grib2
 constant_field.grib2
"

if [ $HAVE_AEC -eq 1 ]; then
 files="ccsds.grib2 "$files
fi

if [ $HAVE_JPEG -eq 1 ]; then
 files="jpeg.grib2 reduced_gaussian_surface_jpeg.grib2 "$files
fi

# TODO: For the following the PNG packing fails!
#  data/sample.grib2
#  data/missing.grib2
#  data/tigge_af_ecmwf.grib2

for file in $files; do
  infile=${data_dir}/$file
  ${tools_dir}/grib_set -r -s packingType=grid_png $infile $temp >/dev/null

  ${tools_dir}/grib_get '-F%.2f' -p min,max,avg $infile > $temp1
  ${tools_dir}/grib_get '-F%.2f' -p min,max,avg $temp   > $temp2
  diff $temp1 $temp2

  rm -f $temp $temp1 $temp2
done

infile=${data_dir}/sample.grib2
set +e
${tools_dir}/grib_set -r -s packingType=grid_png $infile $temp > $tempErr 2>&1
set -e
grep -q "Unable to set double array 'codedValues'" $tempErr

# Nearest neighbour
# ----------------------
infile=${data_dir}/reduced_gaussian_model_level.grib2
${tools_dir}/grib_set -r -s packingType=grid_png $infile $temp
${tools_dir}/grib_get -F%.6g -l 48.835,327.600,1 $temp > $temp1
grep -q "224.455" $temp1

${tools_dir}/grib_ls -F%.6g -l 48.835,327.600 $temp > $temp1
grep -q "Grid Point chosen #4 index=936 " $temp1

# jPointsAreConsecutive
infile=${data_dir}/reduced_gaussian_model_level.grib2
${tools_dir}/grib_set -r -s jPointsAreConsecutive=1,packingType=grid_png $infile $temp


# Conversion from IEEE to PNG
# ----------------------------
infile=${data_dir}/grid_ieee.grib
${tools_dir}/grib_set -r -s packingType=grid_png $infile $temp
# TODO: check results
grib_check_key_equals $temp packingType grid_png
grib_check_key_equals $temp accuracy 0


# Clean up
rm -f $temp $temp1 $temp2 $tempErr

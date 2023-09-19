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

label="grib_headers_only_test"
tempLog=temp.$label.log
tempText=temp.$label.txt
rm -f $tempText $tempLog

sample_g1="$ECCODES_SAMPLES_PATH/GRIB1.tmpl"
sample_g2="$ECCODES_SAMPLES_PATH/GRIB2.tmpl"

files="$sample_g1 $sample_g2"
for f in $files; do
  # Since we're not including the data section, the packingType key must not appear
  ${tools_dir}/grib_ls -j -M -x $f > $tempLog
  set +e
  grep -q "packingType" $tempLog
  status=$?
  set -e
  [ $status -ne 0 ]
done

${tools_dir}/grib_dump -M -x -O $sample_g1 > $tempLog
${tools_dir}/grib_dump -M -x -O $sample_g2 > $tempLog


files="reduced_gaussian_lsm.grib1
reduced_gaussian_model_level.grib1
reduced_gaussian_model_level.grib2
reduced_gaussian_pressure_level.grib1
reduced_gaussian_pressure_level.grib2
reduced_gaussian_surface.grib1
reduced_gaussian_surface.grib2
reduced_latlon_surface.grib1
reduced_latlon_surface.grib2
regular_gaussian_pressure_level.grib1
regular_gaussian_pressure_level.grib2
regular_gaussian_surface.grib1
regular_gaussian_surface.grib2
regular_latlon_surface.grib1
regular_latlon_surface.grib2
"

for f in $files; do
  file=$data_dir/$f
  echo $file
  ${tools_dir}/grib_ls -M -x $file > $tempLog
  ${tools_dir}/grib_dump -O -M -x $file > $tempLog
done

# Clean up
rm -f $tempText $tempLog

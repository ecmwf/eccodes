#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null
label="grib_png"
temp=${label}".grib.tmp"
temp1=${label}".1.tmp"
temp2=${label}".2.tmp"

files="
 reduced_gaussian_model_level.grib2
 reduced_gaussian_sub_area.grib2
 regular_latlon_surface_constant.grib2
"

if [ $HAVE_JPG -eq 1 ]; then
 files="multi.grib2 v.grib2"$files
fi

# TODO: For the following the PNG packing fails with an assert!
#       grib_accessor_class_data_png_packing.c: Assert(p->offset + length <= p->length)
#  data/sample.grib2
#  data/missing.grib2
#  data/tigge_af_ecmwf.grib2

for file in $files; do
  infile=${data_dir}/$file
  ${tools_dir}grib_set -r -s packingType=grid_png $infile $temp >/dev/null

  ${tools_dir}grib_get '-F%.2f' -p min,max,avg $infile > $temp1
  ${tools_dir}grib_get '-F%.2f' -p min,max,avg $temp   > $temp2
  diff $temp1 $temp2

  rm -f $temp $temp1 $temp2
done

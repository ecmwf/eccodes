#!/bin/sh
# Copyright 2005-2013 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#


. ./include.sh

files="regular_latlon_surface.grib2 \
       regular_latlon_surface.grib1"

# Create the grib2 file from grib1
${tools_dir}grib_set -s editionNumber=2 ${data_dir}/regular_latlon_surface.grib1 ${data_dir}/regular_latlon_surface.grib2


for file in `echo $files`; do

  infile=${data_dir}/$file
  outfile1=${infile}_bitsPerValue_1
  outfile2=${infile}_bitsPerValue_2

  ${tools_dir}grib_set -r -s bitsPerValue=10 $infile $outfile1

  ${tools_dir}grib_set -s setBitsPerValue=10 $infile $outfile2

  ${tools_dir}grib_compare $outfile1 $outfile2

  rm -f $outfile1 $outfile2 || true

done

# Extra test for grid_complex_spatial_differencing
res=`${tools_dir}grib_get -p decimalScaleFactor,bitsPerValue ${data_dir}/gfs.c255.grib2`
[ "$res" = "1 20" ]

rm -f ${data_dir}/regular_latlon_surface.grib2

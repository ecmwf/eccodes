#!/bin/sh
# Copyright 2005-2014 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

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

# First create the necessary grib2 files from grib1
files2conv="\
 reduced_latlon_surface\
 reduced_gaussian_pressure_level\
 regular_gaussian_pressure_level\
 reduced_gaussian_sub_area\
 regular_latlon_surface\
"
for f in $files2conv; do
  ${tools_dir}grib_set -s editionNumber=2 ${data_dir}/$f.grib1 ${data_dir}/$f.grib2
done

# First create the necessary grib2 files
#${tools_dir}grib_set -s editionNumber=2 ${data_dir}/reduced_latlon_surface.grib1 ${data_dir}/reduced_latlon_surface.grib2
#${tools_dir}grib_set -s editionNumber=2 ${data_dir}/reduced_gaussian_pressure_level.grib1 ${data_dir}/reduced_gaussian_pressure_level.grib2
#${tools_dir}grib_set -s editionNumber=2 ${data_dir}/regular_gaussian_pressure_level.grib1 ${data_dir}/regular_gaussian_pressure_level.grib2
#${tools_dir}grib_set -s editionNumber=2 ${data_dir}/reduced_gaussian_sub_area.grib1 ${data_dir}/reduced_gaussian_sub_area.grib2

for f in `echo $files`
do
 file=${data_dir}/$f
 iterator_count=`${tools_dir}grib_get_data -m 9999:missing -f -p centre -F "%g" -w count=1 $file | grep -v Lat |wc -l `
 numberOfPoints=`${tools_dir}grib_get -w count=1 -p numberOfPoints $file`
 [ $numberOfPoints = ${iterator_count} ]
done

# Clean up
for f in $files2conv; do
  rm -f ${data_dir}/$f.grib2
done

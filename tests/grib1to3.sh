#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

label="grib1to3"
temp=temp.$label.grib3
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
${tools_dir}/grib_set -s editionNumber=3 $sample_g1 $temp
${tools_dir}/grib_dump -O -M $temp
rm -f $temp

# A set of GRIB1 files
#files="constant_field\
# reduced_gaussian_pressure_level_constant \
# reduced_latlon_surface_constant \
# regular_gaussian_pressure_level_constant \
# regular_latlon_surface_constant \
# reduced_latlon_surface \
# reduced_gaussian_pressure_level \
# reduced_gaussian_sub_area \
# regular_gaussian_pressure_level \
# regular_latlon_surface \
# reduced_gaussian_model_level \
# regular_gaussian_model_level \
# reduced_gaussian_surface \
# regular_gaussian_surface \
# spherical_pressure_level \
# spherical_model_level "

files="constant_field\
 regular_latlon_surface_constant \
 regular_latlon_surface \
"

for f in $files
do
  file=${data_dir}/$f
  output=${file}.temp.$label.grib3_
  rm -f ${output}
  ${tools_dir}/grib_set -s editionNumber=3 ${file}.grib1 ${output}

  #statsBefore=`${tools_dir}/grib_get -fp numberOfValues,numberOfPoints,max,min,average,numberOfMissing ${file}.grib1` 
  #statsAfter=`${tools_dir}/grib_get -M -fp numberOfValues,numberOfPoints,max,min,average,numberOfMissing ${output}` 

  #if [ "$statsBefore" != "$statsAfter" ]; then 
  #  exit 1
  #fi

  #${tools_dir}/grib_compare -A1.0e-8 -c values ${file}.grib1 ${output} 2> /dev/null > /dev/null
  ${tools_dir}/grib_compare -P -c values ${file}.grib1 ${output} 2> $REDIRECT > $REDIRECT

  rm -f ${output}

done

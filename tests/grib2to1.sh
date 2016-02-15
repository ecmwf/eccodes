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

files="constant_field\
 reduced_gaussian_pressure_level_constant \
 reduced_latlon_surface_constant \
 regular_gaussian_pressure_level_constant \
 regular_latlon_surface_constant \
 reduced_latlon_surface \
 reduced_gaussian_pressure_level \
 reduced_gaussian_sub_area \
 regular_gaussian_pressure_level \
 regular_latlon_surface \
 reduced_gaussian_model_level \
 regular_gaussian_model_level \
 reduced_gaussian_surface \
 regular_gaussian_surface \
 spherical_pressure_level \
 spherical_model_level "

for f in $files
do
  file=${data_dir}/$f
  output=${file}.grib1_
  rm -f ${output} || true
  ${tools_dir}grib_set -s editionNumber=1 ${file}.grib2 ${output} 2> $REDIRECT > $REDIRECT

  grib1Statistics=`${tools_dir}grib_get -fp numberOfValues,numberOfPoints,max,min,average,numberOfMissing ${output}` 
  grib2Statistics=`${tools_dir}grib_get -fp numberOfValues,numberOfPoints,max,min,average,numberOfMissing ${file}.grib2` 

  if [ "$grib1Statistics" != "$grib2Statistics" ]; then 
    exit 1
  fi

  #${tools_dir}grib_compare -A1.0e-8 -c values ${output} ${file}.grib2 2> /dev/null > /dev/null
  rm -f ${output}
done

# GRIB-262 Conversion works without error for L137 data
# First create a grib2 file with NV () > 255 which should not be convertible to grib1
filter=temp.setpv.filt
COUNT=264
rm -f $filter || true
echo "set NV=$COUNT;" >> $filter
echo "set pv={"       >> $filter
i=1
while [ $i -le $COUNT ]; do
  if [ $i = $COUNT ]; then
    echo " $i"        >> $filter
  else
    echo " $i ,"      >> $filter
  fi
  i=`expr $i + 1`
done
echo "};write;"       >> $filter
# Apply this filter to a grib2 file from samples.
${tools_dir}grib_filter -o temp.pv.grib2 $filter $ECCODES_SAMPLES_PATH/reduced_gg_ml_grib2.tmpl
# Convert this new grib2 file to grib1. This command SHOULD FAIL
set +e
${tools_dir}grib_set -s edition=1 temp.pv.grib2 temp.bad.grib1 2>$REDIRECT
if [ $? -eq 0 ]; then
  echo "ERROR: Conversion from grib2 to grib1 should have failed for large NV!" >&2
  exit 1
fi
set -e
rm -f $filter temp.pv.grib2 || true

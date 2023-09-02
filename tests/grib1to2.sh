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

for f in `echo $files`
do
  file=${data_dir}/$f
  output=${file}.grib2_
  rm -f ${output}
  ${tools_dir}/grib_set -s editionNumber=2 ${file}.grib1 ${output} 2> $REDIRECT > $REDIRECT

  grib1Statistics=`${tools_dir}/grib_get -fp numberOfValues,numberOfPoints,max,min,average,numberOfMissing ${file}.grib1` 
  grib2Statistics=`${tools_dir}/grib_get -fp numberOfValues,numberOfPoints,max,min,average,numberOfMissing ${output}` 

  if [ "$grib1Statistics" != "$grib2Statistics" ]; then 
    exit 1
  fi

  #${tools_dir}/grib_compare -A1.0e-8 -c values ${file}.grib1 ${output} 2> /dev/null > /dev/null
  ${tools_dir}/grib_compare -P -c values ${file}.grib1 ${output} 2> $REDIRECT > $REDIRECT
  
  rm -f ${output}

done

echo "ECC-457,ECC-1298 ECMWF total precipitation..."
# ---------------------------------------------------
input=${data_dir}/tp_ecmwf.grib
output=temp.grib1to2.grib
${tools_dir}/grib_set -s edition=2 $input $output
res=`${tools_dir}/grib_get -w count=1 -p edition,paramId,units $output`
[ "$res" = "2 228 m" ]
res=`${tools_dir}/grib_get -w count=1 -p stepType $output`
[ "$res" = "accum" ]
rm -f $output


echo "Local Definition 30..."
# ----------------------------
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=30,stepType=accum,edition=2 $sample_g1 $output
grib_check_key_equals $output productDefinitionTemplateNumber 11
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=30,type=em,stepType=accum,edition=2 $sample_g1 $output
grib_check_key_equals $output productDefinitionTemplateNumber 12
rm -f $output


echo "Check global gaussian grids are preserved..."
# -------------------------------------------------
# Input is global gaussian so converted output should also be global with the correct lat/lon
input=$data_dir/reduced_gaussian_surface.grib1
${tools_dir}/grib_set -s edition=2 $input $output
grib_check_key_equals $output latitudeOfFirstGridPoint,longitudeOfLastGridPoint '87863799 357187500'
if [ -x "${tools_dir}/grib_check_gaussian_grid" ]; then
    ${tools_dir}/grib_check_gaussian_grid $input $output
fi


echo "ECC-811 Check shapeOfTheEarth..."
# ---------------------------------------------------
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
${tools_dir}/grib_set -s edition=2 $sample_g1 $output
grib_check_key_equals $sample_g1 shapeOfTheEarth 0
grib_check_key_equals $output    shapeOfTheEarth 0


echo "ECC-1329: Cannot convert runoff (paramId=205)"
# --------------------------------------------------------
temp1="temp1.grib1to2.grib1"
temp2="temp2.grib1to2.grib2"
${tools_dir}/grib_set -s paramId=205,P1=240,marsType=fc $sample_g1 $temp1
${tools_dir}/grib_set -s edition=2 $temp1 $temp2
grib_check_key_equals $temp2 discipline,stepType,shortName,paramId '2 accum ro 205'
# Fix the stepRange too - TODO
${tools_dir}/grib_set -s edition=2,startStep=0 $temp1 $temp2
grib_check_key_equals $temp2 stepType,stepRange 'accum 0-240'
rm -f $temp1 $temp2

echo "ECC-1646: Cannot convert sro, uvb, lsp, e, and pev"
# --------------------------------------------------------
for sn in e lsp pev sro uvb; do
    ${tools_dir}/grib_set -s shortName=$sn,typeOfLevel=surface,level=0 $sample_g1 $temp1
    ${tools_dir}/grib_set -s edition=2 $temp1 $temp2
    ${tools_dir}/grib_compare -e -b param $temp1 $temp2
done

# Turn on (brief) DEBUGGING messages
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
output=temp.grib1to2.grib
ECCODES_DEBUG=-1 ${tools_dir}/grib_set -s edition=2 $sample_g1 $output


# Clean up
rm -f $output
rm -f $temp1 $temp2

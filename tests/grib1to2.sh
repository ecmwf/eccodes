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

# ECC-457 ECMWF total precipitation
# ----------------------------------
input=${data_dir}/tp_ecmwf.grib
output=temp.grib1to2.grib
${tools_dir}/grib_set -s edition=2 $input $output
res=`${tools_dir}/grib_get -w count=1 -p edition,paramId,units $output`
[ "$res" = "2 228228 kg m**-2" ]
rm -f $output

# Local Definition 30
# --------------------
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=30,stepType=accum,edition=2 $sample_g1 $output
grib_check_key_equals $output productDefinitionTemplateNumber 11
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=30,type=em,stepType=accum,edition=2 $sample_g1 $output
grib_check_key_equals $output productDefinitionTemplateNumber 12
rm -f $output

#sed "s:toolsdir:${tools_dir}/:" ${tools_dir}/grib1to2.txt > ${tools_dir}/grib1to2.test
#chmod +x ${tools_dir}/grib1to2.test
#${tools_dir}/grib1to2.test -f ${data_dir}/test.grib1 ${data_dir}/test.grib2
#${tools_dir}/grib_get -p typeOfProcessedData:s,type:s ${data_dir}/test.grib2 > ${data_dir}/typeOfProcessedData.log

#diff ${data_dir}/typeOfProcessedData.log ${data_dir}/typeOfProcessedData.ok

#${tools_dir}/grib_compare -Pe ${data_dir}/test.grib1 ${data_dir}/test.grib2

#rm -f ${data_dir}/test.grib2

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

label="grib_efas"
sample=temp.sample.$label.grib
temp1=temp1.$label.grib
temp2=temp2.$label.grib
temp3=temp3.$label.grib


# Create a starting GRIB with a basic local definition for MARS
${tools_dir}/grib_set -s tablesVersion=19,setLocalDefinition=1,stream=efas $ECCODES_SAMPLES_PATH/GRIB2.tmpl $sample

# Check is_efas key
grib_check_key_equals $sample is_efas 0

# Test a non-ensemble, instantaneous field
${tools_dir}/grib_set -s productDefinitionTemplateNumber=0 $sample $temp1
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=41 $temp1 $temp2
grib_check_key_equals $temp2 is_efas,productDefinitionTemplateNumber '1 70'
grib_check_key_exists $temp2 isFillup,dateOfForecast,timeOfForecast,anoffset

# Test an ensemble, instantaneous field
${tools_dir}/grib_set -s productDefinitionTemplateNumber=1,number=13 $sample $temp1
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=41 $temp1 $temp2
grib_check_key_equals $temp2 is_efas,number,productDefinitionTemplateNumber '1 13 71'

# Test a non-ensemble, non-instantaneous field
${tools_dir}/grib_set -s productDefinitionTemplateNumber=8,stepType=accum $sample $temp1
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=41 $temp1 $temp2
grib_check_key_equals $temp2 is_efas,productDefinitionTemplateNumber,typeOfStatisticalProcessing '1 72 1'

# Test an ensemble, non-instantaneous field (plus mars.origin tests)
${tools_dir}/grib_set -s productDefinitionTemplateNumber=11,stepType=accum $sample $temp1
${tools_dir}/grib_set -s \
    setLocalDefinition=1,localDefinitionNumber=41,type=pf,inputOriginatingCentre=ecmf,typeOfPostProcessing=1 \
    $temp1 $temp2
grib_check_key_equals $temp2 is_efas,productDefinitionTemplateNumber,typeOfStatisticalProcessing '1 73 1'
grib_check_key_equals $temp2 mars.origin 'ecmf'
grib_check_key_equals $temp2 mars.model  'lisflood'

# Parameter tests
${tools_dir}/grib_set -s paramId=260267 $temp2 $temp3
grib_check_key_equals $temp3 paramId,is_efas,lengthOfTimeRange '260267 1 6'

${tools_dir}/grib_set -s paramId=260268 $temp2 $temp3
grib_check_key_equals $temp3 paramId,is_efas,lengthOfTimeRange '260268 1 24'

# Use stepType
${tools_dir}/grib_set -s localDefinitionNumber=41,stepType=accum $sample $temp1
${tools_dir}/grib_set -s paramId=260267 $temp1 $temp2

# Test anoffset calculation
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=41,yearOfForecast=2007,monthOfForecast=3,dayOfForecast=24,hourOfForecast=13 \
  $sample $temp1
grib_check_key_equals $temp1 anoffset 25
grib_check_key_equals $temp1 anoffsetFirst,anoffsetLast,anoffsetFrequency "MISSING MISSING MISSING"

# MARS step
types="sfo fu go"
for t in $types; do
  ${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=41,type=$t,stepType=accum,stepRange=12-36,paramId=260268 \
  $sample $temp1
  grib_check_key_equals $temp1 mars.step 36 # end step (ECC-701)
  grib_check_key_exists $temp1 mars.anoffset
  #${tools_dir}/grib_dump -Da $temp1 | grep mars.step
done

types="fc pf cf"
for t in $types; do
  ${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=41,type=$t,stepType=accum,stepRange=12-36,paramId=260268 \
  $sample $temp1
  grib_check_key_equals $temp1 mars.step 36 # end step
  #${tools_dir}/grib_dump -Da $temp1 | grep mars.step
done


# Clean up
rm -f $sample $temp1 $temp2 $temp3

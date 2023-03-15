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

label="grib_true_imagery_test"
temp=temp.$label
tempd=temp_dump.$label
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

latest=30 # TODO

## PDTN 108 - Analysis or forecast at a horizontal level or in a horizontal layer at a point in time for generic optical products
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=108 $sample_grib2 $temp
${tools_dir}/grib_dump -O $temp > $tempd
grep -q 'Analysis or forecast at a horizontal level or in a horizontal layer at a point in time for generic optical properties' $tempd
grib_check_key_equals $temp typeOfWavelengthInterval,scaledValueOfFirstWavelength,scaledValueOfSecondWavelength "0 0 MISSING"

## PDTN 109 - Individual ensemble forecast, control and perturbed, at a horizontal level or in a horizontal layer at a point in time for generic optical products
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=109 $sample_grib2 $temp
${tools_dir}/grib_dump -O $temp > $tempd
grep -q 'Individual ensemble forecast, control and perturbed, at a horizontal level or in a horizontal layer at a point in time for generic optical properties' $tempd
grib_check_key_equals $temp typeOfWavelengthInterval,scaledValueOfFirstWavelength,scaledValueOfSecondWavelength "0 0 MISSING"

## PDTN 110 - Average, accumulation, extreme values or other statistically processed values at a horizontal level or in a horizontal layer in a continuous or non-continuous time interval for generic optical products
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=110 $sample_grib2 $temp
${tools_dir}/grib_dump -O $temp > $tempd
grep -q 'Average, accumulation, and/or extreme values or other statistically processed values at a horizontal level or in a horizontal layer in a continuous or non-continuous time interval for generic optical properties' $tempd
grib_check_key_equals $temp typeOfWavelengthInterval,scaledValueOfFirstWavelength,scaledValueOfSecondWavelength "0 0 MISSING"

## PDTN 111 - Average, accumulation, extreme values or other statistically processed values at a horizontal level or in a horizontal layer in a continuous or non-continuous time interval for generic optical products
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=111 $sample_grib2 $temp
${tools_dir}/grib_dump -O $temp > $tempd
grep -q 'Individual ensemble forecast, control and perturbed, at a horizontal level or in a horizontal layer in a continuous or non-continuous time interval for generic optical properties' $tempd
grib_check_key_equals $temp typeOfWavelengthInterval,scaledValueOfFirstWavelength,scaledValueOfSecondWavelength "0 0 MISSING"

rm -f $temp $tempd

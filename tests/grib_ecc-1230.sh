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

label="prod_ECC-1230_test"
temp1=temp1.$label
temp2=temp2.$label
sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl

# paramId 228 is "Total precipitation" (=tp)
# ------------------------------------------
${tools_dir}/grib_set -s \
type=pf,localDefinitionNumber=30,stream=enfo,\
perturbationNumber=10,numberOfForecastsInEnsemble=51,indicatorOfParameter=228 \
$sample_grib1 $temp1
grib_check_key_equals $temp1 "stepTypeForConversion" "accum"
grib_check_key_equals $temp1 "paramId,shortName,productDefinitionTemplateNumber" "228 tp 11"

# Convert
${tools_dir}/grib_set -s edition=2 $temp1 $temp2
grib_check_key_equals $temp2 "productDefinitionTemplateNumber" "11"
grib_check_key_equals $temp2 "numberOfForecastsInEnsemble,perturbationNumber" "51 10"


# Now check 2m temperature (not accumulated)
# ------------------------------------------
${tools_dir}/grib_set -s \
type=pf,localDefinitionNumber=30,stream=enfo,\
perturbationNumber=10,numberOfForecastsInEnsemble=51,shortName=2t \
$sample_grib1 $temp1
grib_check_key_equals $temp1 "productDefinitionTemplateNumber" "1"
# Convert
${tools_dir}/grib_set -s edition=2 $temp1 $temp2
grib_check_key_equals $temp2 "productDefinitionTemplateNumber" "1"
grib_check_key_equals $temp2 "numberOfForecastsInEnsemble,perturbationNumber" "51 10"


# Clean up
rm -f $temp1 $temp2

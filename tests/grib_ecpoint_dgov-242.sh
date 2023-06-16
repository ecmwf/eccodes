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

label="grib_ecPoint_DGOV-242_test"  # Change prod to bufr or grib etc
tempGrib=temp.$label.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# 1. bias-corrected
## we check if we can set PDTN 73 and typeOfPostProcessing to ecPoint, ecPoint versioning and MARS type
${tools_dir}/grib_set -s tablesVersion=33,productDefinitionTemplateNumber=73,backgroundProcess=1,\
setLocalDefinition=1,grib2LocalSectionNumber=1,type=gbf,class=od,stream=enfo,\
generatingProcessIdentifier=2,typeOfPostProcessing=206 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib backgroundProcess,generatingProcessIdentifier,typeOfPostProcessing,type "1 2 206 gbf"
rm -f $tempGrib

# 2. Point values
## we check if we can set PDTN 122 and typeOfPostProcessing to ecPoint, ecPoint versioning, MARS type, quantile
${tools_dir}/grib_set -s tablesVersion=33,productDefinitionTemplateNumber=122,backgroundProcess=1,\
setLocalDefinition=1,grib2LocalSectionNumber=1,type=pfc,class=od,stream=enfo,\
totalNumberOfQuantiles=1000,quantileValue=995,\
generatingProcessIdentifier=2,typeOfPostProcessing=206 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib backgroundProcess,generatingProcessIdentifier,typeOfPostProcessing,type,quantile "1 2 206 pfc 995:1000"
rm -f $tempGrib

# 3. Point value metrics
## we check if we can set PDTN 122 and typeOfPostProcessing to ecPoint, ecPoint versioning, MARS type, quantile
${tools_dir}/grib_set -s tablesVersion=33,productDefinitionTemplateNumber=122,backgroundProcess=1,\
setLocalDefinition=1,grib2LocalSectionNumber=1,type=ppm,class=od,stream=enfo,\
totalNumberOfQuantiles=1000,quantileValue=995,\
generatingProcessIdentifier=2,typeOfPostProcessing=206 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib backgroundProcess,generatingProcessIdentifier,typeOfPostProcessing,type,quantile "1 2 206 ppm 995:1000"
rm -f $tempGrib

# 4. weather types
## we check if we can set PDTN 73 and typeOfPostProcessing to ecPoint, ecPoint versioning and MARS type
${tools_dir}/grib_set -s tablesVersion=33,productDefinitionTemplateNumber=73,backgroundProcess=1,\
setLocalDefinition=1,grib2LocalSectionNumber=1,type=gwt,class=od,stream=enfo,\
generatingProcessIdentifier=2,typeOfPostProcessing=206 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib backgroundProcess,generatingProcessIdentifier,typeOfPostProcessing,type "1 2 206 gwt"
rm -f $tempGrib

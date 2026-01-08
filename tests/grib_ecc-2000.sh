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

# ---------------------------------------------------------
# This is the test for JIRA issue ECC-XXXX
# < Add issue summary here >
# ---------------------------------------------------------

REDIRECT=/dev/null

label="prod_ecc-2000_test"  # Change prod to bufr or grib etc
tempGrib1=temp1.$label.grib
tempGrib2=temp2.$label.grib
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s setLocalDefinition=1,marsStream=1035,marsType=38,productDefinitionTemplateNumber=107,numberOfAdditionalParametersForReferencePeriod=2 $sample_grib2 $tempGrib1

# SOT case with percentiles 90 and 99

cat >$tempFilt<<EOF
#set setLocalDefinition=1;
#set stream=1035;
#set type=38;
#set productDefinitionTemplateNumber = 107;
set forecastTime = 72;
set lengthOfTimeRange = 120;
set typeOfStatisticalProcessing = 102;
set typeOfRelationToReferenceDataset = 21;
#set numberOfAdditionalParametersForReferencePeriod = 2 ;
set scaleFactorOfAdditionalParameterForReferencePeriod = {0,0};
set scaledValueOfAdditionalParameterForReferencePeriod = {90,99};
write;
EOF

${tools_dir}/grib_filter -o $tempGrib2 $tempFilt $tempGrib1

grib_check_key_equals $tempGrib2  "mars.quantile" "90-99:100"


# SOT case with percentiles 10 and 1
cat >$tempFilt<<EOF
#set setLocalDefinition=1;
#set stream=1035;
#set type=38;
#set productDefinitionTemplateNumber = 107;
set forecastTime = 72;
set lengthOfTimeRange = 120;
set typeOfStatisticalProcessing = 102;
set typeOfRelationToReferenceDataset = 21;
#set numberOfAdditionalParametersForReferencePeriod = 2 ;
set scaleFactorOfAdditionalParameterForReferencePeriod = {0,0};
set scaledValueOfAdditionalParameterForReferencePeriod = {10,1};
write;
EOF

${tools_dir}/grib_filter -o $tempGrib2 $tempFilt $tempGrib1

grib_check_key_equals $tempGrib2  "mars.quantile" "10-1:100"


# SOT case with percentiles 90 and 99.5
cat >$tempFilt<<EOF
#set setLocalDefinition=1;
#set stream=1035;
#set type=38;
#set productDefinitionTemplateNumber = 107;
set forecastTime = 72;
set lengthOfTimeRange = 120;
set typeOfStatisticalProcessing = 102;
set typeOfRelationToReferenceDataset = 21;
#set numberOfAdditionalParametersForReferencePeriod = 2 ;
set scaleFactorOfAdditionalParameterForReferencePeriod = {0,1};
set scaledValueOfAdditionalParameterForReferencePeriod = {90,995};
write;
EOF

${tools_dir}/grib_filter -o $tempGrib2 $tempFilt $tempGrib1

grib_check_key_equals $tempGrib2  "mars.quantile" "900-995:1000"



# Clean up
rm -f $tempGrib1 $tempGrib2  $tempFilt

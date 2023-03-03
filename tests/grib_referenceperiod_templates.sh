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
set -u
REDIRECT=/dev/null
label="grib_referenceperiod_templates_test"  # Change prod to bufr or grib etc
temp=temp.$label
tempd=temp_dump.$label
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

#latest=`${tools_dir}/grib_get -p tablesVersionLatest $sample2`
latest=30 # TODO

## PDTN 105 - Anomalies, significance and other derived products from an analysis or forecast in relation to a reference period at a horizontal level or in a horizontal layer in a continuous or non-continuous time interval
# we create a sample file of the template
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=105 $sample_grib2 $temp
${tools_dir}/grib_dump -O $temp > $tempd
grep -q 'Analysis or forecast in relation to a reference period' $tempd
grib_check_key_equals $temp typeOfReferenceDataset,typeOfRelationToReferenceDataset,numberOfReferencePeriodTimeRanges "255 255 1"
# we check if the section 4 has the expected section4length for the default keys
grib_check_key_equals $temp section4Length "79"
# 
${tools_dir}/grib_filter -o $temp - $sample_grib2 <<EOF
set tablesVersion=30;
set productDefinitionTemplateNumber=105;
set numberOfTimeRange = 1 ;
set numberOfReferencePeriodTimeRanges = 2;
set numberOfAdditionalParametersForReferencePeriod = 3 ;
write;
EOF
grib_check_key_equals $temp section4Length "100"


## PDTN 106 - Anomalies, significance and other derived products from an individual ensemble Forecast, control and perturbed in relation to a reference period at a horizontal level or in a horizontal layer in a continuous or non-continuous time interval
# we create a sample file of the template
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=106 $sample_grib2 $temp
${tools_dir}/grib_dump -O $temp > $tempd
grep -q 'Individual ensemble forecast, control and perturbed in relation to a reference period' $tempd
grib_check_key_equals $temp typeOfReferenceDataset,typeOfRelationToReferenceDataset,numberOfReferencePeriodTimeRanges "255 255 1"
# we check if the section 4 has the expected section4length
grib_check_key_equals $temp section4Length "82"
#
${tools_dir}/grib_filter -o $temp - $sample_grib2 <<EOF
set tablesVersion=30;
set productDefinitionTemplateNumber=106;
set numberOfTimeRange = 1 ;
set numberOfReferencePeriodTimeRanges = 2;
set numberOfAdditionalParametersForReferencePeriod = 3 ;
write;
EOF
grib_check_key_equals $temp section4Length "103"


## PDTN 107 - Anomalies, significance and other derived products from derived Forecasts based on all ensemble members in relation to a reference period at a horizontal level or in a horizontal layer in a continuous or non-continuous time interval
# we create a sample file of the template
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=107 $sample_grib2 $temp
${tools_dir}/grib_dump -O $temp > $tempd
grep -q 'Derived forecasts based on all ensemble members in relation to a reference period' $tempd
grib_check_key_equals $temp typeOfReferenceDataset,typeOfRelationToReferenceDataset,numberOfReferencePeriodTimeRanges "255 255 1"
# we check if the section 4 has the expected section4length
grib_check_key_equals $temp section4Length "81"
#
${tools_dir}/grib_filter -o $temp - $sample_grib2 <<EOF
set tablesVersion=30;
set productDefinitionTemplateNumber=107;
set numberOfTimeRange = 1 ;
set numberOfReferencePeriodTimeRanges = 2;
set numberOfAdditionalParametersForReferencePeriod = 3 ;
write;
EOF
grib_check_key_equals $temp section4Length "102"


## PDTN 112 - Probability of anomalies, significance and other derived products from an ensemble forecast in relation to a reference period at a horizontal level or in a horizontal layer in a continuous or non-continuous time interval
# we create a sample file of the template
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=112 $sample_grib2 $temp
${tools_dir}/grib_dump -O $temp > $tempd
grep -q 'Probability of anomalies, significance and other derived products from an ensemble forecast in relation to a reference period' $tempd
grib_check_key_equals $temp typeOfReferenceDataset,typeOfRelationToReferenceDataset,numberOfReferencePeriodTimeRanges "255 255 1"
# we check if the section 4 has the expected section4length
grib_check_key_equals $temp section4Length "92"
#
${tools_dir}/grib_filter -o $temp - $sample_grib2 <<EOF
set tablesVersion=30;
set productDefinitionTemplateNumber=112;
set numberOfTimeRange = 1 ;
set numberOfReferencePeriodTimeRanges = 2;
set numberOfAdditionalParametersForReferencePeriod = 3 ;
write;
EOF
grib_check_key_equals $temp section4Length "113"

rm -f $temp $tempd

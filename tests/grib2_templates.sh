#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
. ./include.sh
set -u
REDIRECT=/dev/null

label="grib2_templates"

temp1=temp1.$label.grib2
temp2=temp2.$label.grib2
temp=temp.$label.grib2
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl


# Template 4.86
# -------------
# TODO: Add tablesVersion later...
$tools_dir/grib_set -s productDefinitionTemplateNumber=86,totalNumberOfQuantiles=2 $sample2 $temp
grib_check_key_equals $temp totalNumberOfQuantiles,quantileValue '2 0'


# Template 4.88
# -------------
# TODO: Add tablesVersion later...
$tools_dir/grib_set -s productDefinitionTemplateNumber=88,numberOfForecastsUsedInLocalTime=1 $sample2 $temp
grib_check_key_equals $temp localTimeMethod 255

# Templates 4.76 -> 4.79, 4.80 -> 4.83
# -------------------------------------
$tools_dir/grib_set -s tablesVersion=24,productDefinitionTemplateNumber=76 $sample2 $temp
grib_check_key_equals $temp constituentType,sourceSinkChemicalPhysicalProcess '0 255'

$tools_dir/grib_set -s tablesVersion=24,productDefinitionTemplateNumber=77 $sample2 $temp
grib_check_key_equals $temp constituentType,sourceSinkChemicalPhysicalProcess,perturbationNumber '0 255 0'

$tools_dir/grib_set -s tablesVersion=24,productDefinitionTemplateNumber=78 $sample2 $temp
grib_check_key_equals $temp constituentType,sourceSinkChemicalPhysicalProcess,numberOfMissingInStatisticalProcess '0 255 0'

$tools_dir/grib_set -s tablesVersion=24,productDefinitionTemplateNumber=79 $sample2 $temp
grib_check_key_equals $temp constituentType,sourceSinkChemicalPhysicalProcess,numberOfMissingInStatisticalProcess,perturbationNumber '0 255 0 0'

$tools_dir/grib_set -s tablesVersion=24,productDefinitionTemplateNumber=80 $sample2 $temp
grib_check_key_equals $temp aerosolType,sourceSinkChemicalPhysicalProcess '0 255'

$tools_dir/grib_set -s tablesVersion=24,productDefinitionTemplateNumber=81 $sample2 $temp
grib_check_key_equals $temp aerosolType,sourceSinkChemicalPhysicalProcess,perturbationNumber '0 255 0'

$tools_dir/grib_set -s tablesVersion=24,productDefinitionTemplateNumber=82 $sample2 $temp
grib_check_key_equals $temp sourceSinkChemicalPhysicalProcess 255

$tools_dir/grib_set -s productDefinitionTemplateNumber=83 $sample2 $temp
grib_check_key_equals $temp sourceSinkChemicalPhysicalProcess 255



# WMO have now added template 4.55 and 4.56
# ------------------------------------------
$tools_dir/grib_set -s productDefinitionTemplateNumber=55 $sample2 $temp
$tools_dir/grib_get -p tileClassification,tileIndex  $temp >/dev/null

$tools_dir/grib_set -s productDefinitionTemplateNumber=56 $sample2 $temp
$tools_dir/grib_get -p tileClassification,tileIndex,perturbationNumber $temp >/dev/null


# WMO have now added template 4.53 and 4.54
# ------------------------------------------
$tools_dir/grib_set -s productDefinitionTemplateNumber=53 $sample2 $temp
$tools_dir/grib_get -p partitionNumber $temp >/dev/null

$tools_dir/grib_set -s productDefinitionTemplateNumber=54 $sample2 $temp
$tools_dir/grib_get -p partitionNumber,perturbationNumber $temp >/dev/null

# S2S templates: 4.60 and 4.61
# ------------------------------------------
$tools_dir/grib_set -s tablesVersion=11,productDefinitionTemplateNumber=60 $sample2 $temp                 2>/dev/null
$tools_dir/grib_get -p perturbationNumber,YearOfModelVersion,MonthOfModelVersion,DayOfModelVersion  $temp >/dev/null

$tools_dir/grib_set -s tablesVersion=11,productDefinitionTemplateNumber=61 $sample2 $temp       2>/dev/null
$tools_dir/grib_get -p perturbationNumber,typeOfStatisticalProcessing,HourOfModelVersion $temp  >/dev/null

# GRIB-991 template 4.57
# ------------------------------------------
$tools_dir/grib_set -s tablesVersion=16,productDefinitionTemplateNumber=57,NP=2 $sample2 $temp
$tools_dir/grib_get -p "constituentType,numberOfModeOfDistribution,modeNumber,typeOfDistributionFunction" $temp >/dev/null

# ECC-410 template 4.58
# ------------------------------------------
$tools_dir/grib_set -s tablesVersion=18,productDefinitionTemplateNumber=58,NP=2 $sample2 $temp
$tools_dir/grib_get -p "constituentType,NP,modeNumber,typeOfDistributionFunction,perturbationNumber" $temp >/dev/null


# template 4.67 and 4.68
# ------------------------------------------
$tools_dir/grib_set -s tablesVersion=19,productDefinitionTemplateNumber=67 $sample2 $temp
grib_check_key_exists $temp 'constituentType,timeIncrement'

$tools_dir/grib_set -s tablesVersion=19,productDefinitionTemplateNumber=68 $sample2 $temp
grib_check_key_exists $temp 'constituentType,perturbationNumber'


# Aerosol ensemble interval template. PDT deprecated: 4.47 -> 4.85
$tools_dir/grib_set -s tablesVersion=26,productDefinitionTemplateNumber=11,stepType=accum,paramId=215211 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber,perturbationNumber '85 0'
$tools_dir/grib_set -s tablesVersion=26,productDefinitionTemplateNumber=47 $sample2 $temp1
$tools_dir/grib_set -s tablesVersion=26,productDefinitionTemplateNumber=85 $sample2 $temp2
$tools_dir/grib_compare -b productDefinitionTemplateNumber $temp1 $temp2


rm -f $temp $temp1 $temp2

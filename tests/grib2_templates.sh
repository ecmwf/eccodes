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

label="grib2_templates_test"

temp1=temp1.$label.grib2
temp2=temp2.$label.grib2
temp=temp.$label.grib2
tempFilt=temp.$label.filt
tempText=temp.$label.txt
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Go thru all templates listed in the most recent Code Table 4.0
# and ensure each has a template in section 4
# -----------------------------------------------
latestOfficial=`${tools_dir}/grib_get -p tablesVersionLatestOfficial $sample2`

latest_codetable_file=$ECCODES_DEFINITION_PATH/grib2/tables/$latestOfficial/4.0.table
awk '$1 !~ /#/ && $1 < 65000 {print $1}' $latest_codetable_file | while read pdtn; do
    if [ ! -f "$ECCODES_DEFINITION_PATH/grib2/template.4.$pdtn.def" ]; then
        echo "GRIB2 template for product definition $pdtn does not exist!"
        exit 1
    fi
done

# ECC-1746
# -------------
$tools_dir/grib_set -s tablesVersion=31,productDefinitionTemplateNumber=34 $sample2 $temp
$tools_dir/grib_ls -j -n time $temp > $tempText
grep -q "stepRange.: 0," $tempText
grep -q "validityDate.: 20070323," $tempText

# Template 4.86
# -------------
$tools_dir/grib_set -s tablesVersion=31,productDefinitionTemplateNumber=86,totalNumberOfQuantiles=2 $sample2 $temp
grib_check_key_equals $temp totalNumberOfQuantiles,quantileValue '2 0'


# Template 4.88
# -------------
$tools_dir/grib_set -s tablesVersion=27,productDefinitionTemplateNumber=88,numberOfForecastsUsedInLocalTime=1 $sample2 $temp
grib_check_key_equals $temp localTimeMethod 255
# ECC-1255: check transient is_localtime is not an array
cat > $tempFilt <<EOF
 print "is_localtime=[is_localtime]";
EOF
result=`${tools_dir}/grib_filter $tempFilt $temp`
[ "$result" = "is_localtime=1" ]


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


# ECC-1564: Support local product definition templates for different centres
$tools_dir/grib_set -s productDefinitionTemplateNumber=40033 $sample2 $temp
$tools_dir/grib_set -s productDefinitionTemplateNumber=40034 $sample2 $temp

# ECC-1563: new local section 4 templates for extra large ensembles
$tools_dir/grib_set -s localTablesVersion=1,productDefinitionTemplateNumber=65533,perturbationNumber=2e8 $sample2 $temp
grib_check_key_equals $temp perturbationNumber '200000000'
$tools_dir/grib_dump -O -p section_4 $temp > $tempText
grep -q "Individual member for large ensemble forecast.*point in time" $tempText

$tools_dir/grib_set -s localTablesVersion=1,productDefinitionTemplateNumber=65534,perturbationNumber=2e8 $sample2 $temp
grib_check_key_equals $temp perturbationNumber,typeOfStatisticalProcessing '200000000 255'
$tools_dir/grib_dump -O -p section_4 $temp > $tempText
grep -q "Individual member for large ensemble forecast.*continuous or non-continuous interval" $tempText

# stepType & PDTNs
# --------------------
test_PDTN_conversions()
{
    # Arguments
    # $1 = product def template for instantaneous
    # $2 = product def template for interval-based
    pdtn_instant=$1
    pdtn_interval=$2
    $tools_dir/grib_set -s productDefinitionTemplateNumber=$pdtn_instant,stepType=accum $sample2 $temp
    grib_check_key_equals $temp productDefinitionTemplateNumber $pdtn_interval
    $tools_dir/grib_set -s stepType=instant $temp $temp1
    grib_check_key_equals $temp1 productDefinitionTemplateNumber $pdtn_instant
}

test_PDTN_conversions 2  12
test_PDTN_conversions 3  13
test_PDTN_conversions 4  14
test_PDTN_conversions 5  9
test_PDTN_conversions 6  10
test_PDTN_conversions 40 42
test_PDTN_conversions 41 43
# test_PDTN_conversions 45 85 # TODO
test_PDTN_conversions 57 67
test_PDTN_conversions 58 68
test_PDTN_conversions 71 73

# ECC-1779: Deprecated and experimental templates
# ------------------------------------------------
grib_check_key_equals $sample2 isTemplateDeprecated,isTemplateExperimental '0 0'

$tools_dir/grib_set -s productDefinitionTemplateNumber=44 $sample2 $temp
grib_check_key_equals $temp isTemplateDeprecated,isTemplateExperimental '1 0'

$tools_dir/grib_set -s productDefinitionTemplateNumber=10 $sample2 $temp
grib_check_key_equals $temp isTemplateDeprecated,isTemplateExperimental '0 1'

$tools_dir/grib_set -s gridType=cross_section $sample2 $temp
grib_check_key_equals $temp isTemplateDeprecated,isTemplateExperimental '0 1'
$tools_dir/grib_set -s gridType=time_section $sample2 $temp
grib_check_key_equals $temp isTemplateDeprecated,isTemplateExperimental '0 1'

# Use of eps key (for local section)
# -----------------------------------
input=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
$tools_dir/grib_set -s stepType=accum,eps=1 $input $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '11'

#17 Ensemble mean
#18 Ensemble standard deviation
$tools_dir/grib_set -s type=17,stepType=accum,eps=1 $input $temp
grib_check_key_equals $temp productDefinitionTemplateNumber,derivedForecast '12 0'

$tools_dir/grib_set -s type=18,stepType=accum,eps=1 $input $temp
grib_check_key_equals $temp productDefinitionTemplateNumber,derivedForecast '12 4'

# Chemicals
$tools_dir/grib_set -s paramId=217019,stepType=instant,eps=0 $input $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '40'

$tools_dir/grib_set -s paramId=217019,stepType=instant,eps=1 $input $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '41'

$tools_dir/grib_set -s paramId=217019,stepType=accum,eps=0 $input $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '42'

$tools_dir/grib_set -s paramId=217019,stepType=accum,eps=1 $input $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '43'

# Aerosol 210072
$tools_dir/grib_set -s paramId=210072,stepType=instant,eps=0 $input $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '48'

$tools_dir/grib_set -s paramId=210072,stepType=instant,eps=1 $input $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '45'

#$tools_dir/grib_set -s paramId=210072,stepType=accum,eps=0 $input $temp
#grib_check_key_equals $temp productDefinitionTemplateNumber '42'

#$tools_dir/grib_set -s paramId=210072,stepType=accum,eps=1 $input $temp
#grib_check_key_equals $temp productDefinitionTemplateNumber '43'


# Clean up
rm -f $temp $temp1 $temp2 $tempFilt $tempText

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
#set -x

REDIRECT=/dev/null
sample_g1=$ECCODES_SAMPLES_PATH/reduced_gg_pl_640_grib1.tmpl
sample_g2=$ECCODES_SAMPLES_PATH/reduced_gg_pl_640_grib2.tmpl

cd ${data_dir}
rm -f local.log

# Check all GRIB2 local def files and definitions/grib2/grib2LocalSectionNumber.98.table
# Each number should appear in the table
# -----------------------------------------
g2lds=${ECCODES_DEFINITION_PATH}/grib2/local.98.*.def
for g2ld in $g2lds; do
    bname=`basename $g2ld`
    dnum=`echo $bname | cut -d. -f3`
    grep -q "^$dnum" ${ECCODES_DEFINITION_PATH}/grib2/grib2LocalSectionNumber.98.table
done

# Use of subCentre=98 for other centre GRIBs
# ------------------------------------------
temp=temp.grib_local.grib
${tools_dir}/grib_set -s centre=edzw,subCentre=98,setLocalDefinition=1,localDefinitionNumber=1 \
  $ECCODES_SAMPLES_PATH/GRIB2.tmpl $temp
grib_check_key_equals $temp section2Length,centre,expver,marsClass:i,marsType:i,marsStream:i "17 edzw 0001 1 2 1025"
rm -f $temp


${tools_dir}/grib_set -s edition=2,setLocalDefinition=1 reduced_gaussian_model_level.grib1 loc.grib2
${tools_dir}/grib_set -s setLocalDefinition=1           reduced_gaussian_model_level.grib1 loc.grib1

# Conversion 1->2
# ----------------
for localDefinitionNumber in 1 15 26 30
do
    ${tools_dir}/grib_set -s localDefinitionNumber=$localDefinitionNumber,perturbationNumber=2,numberOfForecastsInEnsemble=50 loc.grib1 eps.grib1
    ${tools_dir}/grib_set -s edition=2 eps.grib1 eps.grib2

    ${tools_dir}/grib_get -p localDefinitionNumber,perturbationNumber,numberOfForecastsInEnsemble,productDefinitionTemplateNumber eps.grib2 >> local.log
    ${tools_dir}/grib_compare -e -b param eps.grib1 eps.grib2

    ${tools_dir}/grib_set -s localDefinitionNumber=$localDefinitionNumber,numberOfForecastsInEnsemble=0 loc.grib1 eps.grib1
    ${tools_dir}/grib_set -s edition=2 eps.grib1 eps.grib2

    ${tools_dir}/grib_get -f -p localDefinitionNumber,perturbationNumber,productDefinitionTemplateNumber eps.grib2 >> local.log
    ${tools_dir}/grib_compare -e -b param eps.grib1 eps.grib2
done

# Local -> local
# ---------------
for localStart in 1 7 9 20 25 26 30
do
    ${tools_dir}/grib_set -s localDefinitionNumber=$localStart loc.grib1 loc1.grib1
    ${tools_dir}/grib_set -s edition=2 loc1.grib1 loc1.grib2
    ${tools_dir}/grib_get -p localDefinitionNumber loc1.grib1 >> local.log
    ${tools_dir}/grib_get -p localDefinitionNumber loc1.grib2 >> local.log
    
    for localEps in 1 15 26 30
    do
        ${tools_dir}/grib_set -s localDefinitionNumber=$localEps,eps=1,perturbationNumber=2,numberOfForecastsInEnsemble=50 loc1.grib2 eps.grib2
        ${tools_dir}/grib_get -p localDefinitionNumber,perturbationNumber,numberOfForecastsInEnsemble,productDefinitionTemplateNumber eps.grib2 >> local.log
    done
done

# Special types/streams
# ----------------------
${tools_dir}/grib_set -s localDefinitionNumber=1,numberOfForecastsInEnsemble=0 loc.grib1 loc1.grib1
${tools_dir}/grib_set -s edition=2 loc1.grib1 loc1.grib2
${tools_dir}/grib_get -f -p localDefinitionNumber,perturbationNumber loc1.grib2 >> local.log
${tools_dir}/grib_set -s type=em loc1.grib2 eps.grib2
${tools_dir}/grib_get -p localDefinitionNumber,productDefinitionTemplateNumber,derivedForecast eps.grib2 >> local.log
${tools_dir}/grib_set -s type=es loc1.grib2 eps.grib2
${tools_dir}/grib_get -p localDefinitionNumber,productDefinitionTemplateNumber,derivedForecast eps.grib2 >> local.log
${tools_dir}/grib_set -s stream=enda loc1.grib2 eps.grib2 #2> $REDIRECT
${tools_dir}/grib_get -p localDefinitionNumber,productDefinitionTemplateNumber eps.grib2 >> local.log

diff local.log local.good.log

rm -f local.log loc.grib1 loc.grib2 loc1.grib1 loc1.grib2 eps.grib1 eps.grib2


# Delete Local Definition
# -----------------------
temp=temp.grib_local.grib
grib_check_key_equals $sample_g1 localUsePresent 1
${tools_dir}/grib_set -s deleteLocalDefinition=1 $sample_g1 $temp
grib_check_key_equals $temp localUsePresent 0

grib_check_key_equals $sample_g2 "localUsePresent,section2Used" "1 1"
${tools_dir}/grib_set -s deleteLocalDefinition=1 $sample_g2 $temp
grib_check_key_equals $temp "localUsePresent,section2Used" "0 0"
rm -f $temp


# Empty local section for GRIB2
# ------------------------------
sample_g2=$ECCODES_SAMPLES_PATH/reduced_gg_pl_640_grib2.tmpl
temp=temp.grib_local.grib
${tools_dir}/grib_set -s setLocalDefinition=1,addEmptySection2=1 $sample_g2 $temp
grib_check_key_equals $temp section2Length 5

sample_g2=$ECCODES_SAMPLES_PATH/reduced_gg_pl_400_grib2.tmpl
grib_check_key_equals $sample_g2 section2Length 17
${tools_dir}/grib_set -s addEmptySection2=1 $sample_g2 $temp
grib_check_key_equals $temp section2Length 5
rm -f $temp


# Local Definition 4: Ocean model data
# ---------------------------------------
${tools_dir}/grib_set -s \
  localDefinitionNumber=4,coordinate2Flag=2,averaging1Flag=1,coordinate1Flag=1,coordinate2Start=1234 \
  $sample_g1 $temp
grib_check_key_equals $temp "mars.levelist,roundedMarsLevelist:d,roundedMarsLevelist:s" "1 1.234 1.234"


# Local Definition 5: Forecast probability data
# ---------------------------------------------
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
temp=temp.grib_local.grib
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=5 $sample_g1 $temp.1
${tools_dir}/grib_set -s \
   forecastProbabilityNumber=2,totalNumberOfForecastProbabilities=25,thresholdIndicator=3,lowerThreshold=54,upperThreshold=56 \
   $temp.1 $temp.2
${tools_dir}/grib_set -s edition=2 $temp.2 $temp.3
# Now check all LD5 keys from grib1 made it into grib2
grib_check_key_equals $temp.3 edition,productDefinitionTemplateNumber "2 5"
grib_check_key_equals $temp.3 forecastProbabilityNumber,totalNumberOfForecastProbabilities "2 25"
grib_check_key_equals $temp.3 probabilityType,scaledValueOfLowerLimit,scaledValueOfUpperLimit "2 54 56"

# ECC-1045
${tools_dir}/grib_set -s localDefinitionNumber=5,lowerThreshold=missing,upperThreshold=missing \
   $sample_g1 $temp.1
grib_check_key_equals $temp.1 'lowerThreshold,upperThreshold' 'MISSING MISSING'


# Local Definition 42 for GRIB2 (LC-WFV)
# ---------------------------------------
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=42,lcwfvSuiteName=9 $sample_g2 $temp
grib_check_key_equals $temp 'mars.origin:s' 'lops'


# Extra key in Local Definition 16 for GRIB1. ECC-679
# ----------------------------------------------------
${tools_dir}/grib_set -s \
   setLocalDefinition=1,localDefinitionNumber=16,numberOfForecastsInEnsemble=51,verifyingMonth=11 \
  $sample_g1 $temp
grib_check_key_equals $temp 'totalNumber' '51'
grib_check_key_equals $temp 'yearOfEndOfOverallTimeInterval,monthOfEndOfOverallTimeInterval,dayOfEndOfOverallTimeInterval' '0 11 30'

# Local Definition 49 for GRIB1
# -----------------------------
${tools_dir}/grib_set -s localDefinitionNumber=49,type=35 $sample_g1 $temp
grib_check_key_equals $temp 'perturbationNumber,numberOfForecastsInEnsemble' '0 0'


# Local Definition 39 and type 'eme' for GRIB2
# --------------------------------------------
${tools_dir}/grib_set -s \
   setLocalDefinition=1,localDefinitionNumber=39,type=eme,stream=elda,componentIndex=11,offsetToEndOf4DvarWindow=12 \
   $sample_g2 $temp
grib_check_key_equals $temp 'mars.number,mars.anoffset' '11 12'
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=39,type=eme,stream=enda,componentIndex=11 $sample_g2 $temp
grib_check_key_equals $temp 'mars.number' '11'


# Local Definition 18 (list of ascii keys)
# ----------------------------------------
${tools_dir}/grib_filter -o $temp - $sample_g1 << EOF
 set setLocalDefinition=1;
 set localDefinitionNumber=18;
 set consensusCount=3;
 set ccccIdentifiers={"kwbc","ecmf","sabm"};
 write;
EOF

result=`echo 'print "[ccccIdentifiers]";' | ${tools_dir}/grib_filter - $temp`
[ "$result" = "kwbc ecmf sabm" ]


# ECC-1413: Local Definition 38
# --------------------------------
${tools_dir}/grib_set -s \
   setLocalDefinition=1,localDefinitionNumber=38,type=4i,stream=elda,iterationNumber=23 \
   $sample_g2 $temp
grib_check_key_equals $temp 'mars.iteration' '23'
${tools_dir}/grib_ls -jm $temp > $temp.1
grep -q "iteration.* 23" $temp.1

# ECC-1540: Local Definition 36
# --------------------------------
${tools_dir}/grib_set -s \
   setLocalDefinition=1,localDefinitionNumber=36,paramId=210170,class=rd,type=4v,stream=elda \
   $sample_g2 $temp
grib_check_key_exists $temp mars.number,constituentType,sourceSinkChemicalPhysicalProcess
${tools_dir}/grib_set -s localDefinitionNumber=36 $temp $temp.1
${tools_dir}/grib_compare $temp $temp.1

# Clean up
rm -f $temp $temp.1 $temp.2 $temp.3

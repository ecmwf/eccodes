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

label=grib_step_test
tempGrb=${data_dir}/temp.$label.out.grib
templog=${data_dir}/temp.$label.log
rm -f $templog $tempGrb

for i in 0 10
do
  for s in 0 1200 600 6000
  do
    for key in stepRange:s startStep endStep
	do
      rm -f $tempGrb
      ${tools_dir}/grib_set -s ${key}=$s ${data_dir}/timeRangeIndicator_${i}.grib $tempGrb
      #echo grib_set -s ${key}=$s ${data_dir}/timeRangeIndicator_${i}.grib $tempGrb
      #grib_get -p step,startStep,endStep,P1,P2,timeRangeIndicator,indicatorOfUnitOfTimeRange ${data_dir}/timeRangeIndicator_${i}.grib $tempGrb 
      ${tools_dir}/grib_get -p mars.step,stepRange,startStep,endStep,P1,P2,timeRangeIndicator,indicatorOfUnitOfTimeRange:l \
                            ${data_dir}/timeRangeIndicator_${i}.grib $tempGrb >> ${templog}
	done
  done
done

i=5
key=stepRange:s
for s in "0-24" "600-1200" "24-48" "36-66"
do
   ${tools_dir}/grib_set -s ${key}=$s ${data_dir}/timeRangeIndicator_${i}.grib $tempGrb
   #echo grib_set -s ${key}=$s ${data_dir}/timeRangeIndicator_${i}.grib $tempGrb
   #grib_ls -p step,startStep,endStep,P1,P2,timeRangeIndicator,indicatorOfUnitOfTimeRange ${data_dir}/timeRangeIndicator_${i}.grib $tempGrb 
   ${tools_dir}/grib_get -p mars.step,stepRange,startStep,endStep,P1,P2,timeRangeIndicator,indicatorOfUnitOfTimeRange:l \
                         ${data_dir}/timeRangeIndicator_${i}.grib $tempGrb >> ${templog}
done

rm -f $tempGrb

# test added for ifs stepType=max,min
${tools_dir}/grib_set -s stepType=max,startStep=3,endStep=6 ${data_dir}/reduced_gaussian_model_level.grib1 $tempGrb
${tools_dir}/grib_get -p mars.step,stepRange,startStep,endStep,P1,P2,timeRangeIndicator,indicatorOfUnitOfTimeRange:l \
                      ${data_dir}/reduced_gaussian_model_level.grib1 $tempGrb >> ${templog}

rm -f $tempGrb

diff ${data_dir}/step.log ${templog}

(${tools_dir}/grib_filter ${data_dir}/step_grib1.filter ${data_dir}/timeRangeIndicator_0.grib > ${templog}) 2>$REDIRECT

diff ${data_dir}/step_grib1.log ${templog}

rm -f ${templog}

# GRIB-180
# ------------
# Set PDT 4.8 where you can find the EndOfOverallTimeInterval keys
grib2File=${data_dir}/reduced_latlon_surface_constant.grib2
${tools_dir}/grib_set -sproductDefinitionTemplateNumber=8 $grib2File ${grib2File}.p8tmp

# 78 hours is 3 days and 6 hours
${tools_dir}/grib_set -s step=78 $grib2File.p8tmp ${grib2File}.tmp
set `${tools_dir}/grib_get -p hourOfEndOfOverallTimeInterval,dayOfEndOfOverallTimeInterval ${grib2File}.tmp`
hourEnd=$1; dayEnd=$2
[ "$hourEnd" = "18" ]
[ "$dayEnd"  = "8" ]

${tools_dir}/grib_set -s step=12 $grib2File.p8tmp ${grib2File}.tmp
set `${tools_dir}/grib_get -p hourOfEndOfOverallTimeInterval,dayOfEndOfOverallTimeInterval ${grib2File}.tmp`
hourEnd=$1; dayEnd=$2
[ "$hourEnd" = "0" ]
[ "$dayEnd"  = "6" ]


# ECC-134 case-sensitivity
# --------------------------
grib1_sample=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
grib2_sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
temp=temp.step.$$.grib
# M is for Month (code 3)
${tools_dir}/grib_set -s indicatorOfUnitOfTimeRange=M $grib1_sample $temp
unit=`${tools_dir}/grib_get -p unitOfTimeRange $temp`
[ "$unit" = "3" ]
${tools_dir}/grib_set -s indicatorOfUnitOfTimeRange=M $grib2_sample $temp
unit=`${tools_dir}/grib_get -p indicatorOfUnitOfTimeRange $temp`
[ "$unit" = "3" ]

# m is for Minute (code 0)
# -------------------------
${tools_dir}/grib_set -s indicatorOfUnitOfTimeRange=m $grib1_sample $temp
unit=`${tools_dir}/grib_get -p unitOfTimeRange $temp`
[ "$unit" = "0" ]
${tools_dir}/grib_set -s indicatorOfUnitOfTimeRange=m $grib2_sample $temp
unit=`${tools_dir}/grib_get -p indicatorOfUnitOfTimeRange $temp`
[ "$unit" = "0" ]

# ECC-457
# ---------
input=${data_dir}/tp_ecmwf.grib
stepRange=`${tools_dir}/grib_get -w count=1 -p stepRange,startStep,endStep,stepType $input`
[ "$stepRange" = "12 12 12 instant" ]

# ECC-1430: stepType: 'severity' and 'mode'
# -----------------------------------------
${tools_dir}/grib_set -s stepType=severity,paramId=260318 $grib2_sample $temp
grib_check_key_equals $temp productDefinitionTemplateNumber,typeOfStatisticalProcessing '8 100'
${tools_dir}/grib_set -s stepType=mode,paramId=260320     $grib2_sample $temp
grib_check_key_equals $temp productDefinitionTemplateNumber,typeOfStatisticalProcessing '8 101'

# ECC-1577: stepType when typeOfTimeIncrement=255
# -----------------------------------------------
${tools_dir}/grib_set -s stepType=accum,typeOfTimeIncrement=255 $grib2_sample $temp
grib_check_key_equals $temp stepType accum
${tools_dir}/grib_set -s stepType=accum $grib2_sample $temp
grib_check_key_equals $temp typeOfTimeIncrement 2

${tools_dir}/grib_set -s stepType=avg,typeOfTimeIncrement=255 $grib2_sample $temp
grib_check_key_equals $temp stepType avg
${tools_dir}/grib_set -s stepType=avg $grib2_sample $temp
grib_check_key_equals $temp typeOfTimeIncrement 3

# Decode/Encode stepRange as an int and double
${tools_dir}/grib_set -s stepType=accum,stepRange=23-28 $grib2_sample $temp
grib_check_key_equals $temp "stepRange:s" "23-28"
grib_check_key_equals $temp "stepRange:i" "28"
grib_check_key_equals $temp "stepRange:d" "28"

${tools_dir}/grib_set -s stepRange:i=24 $grib2_sample $temp
grib_check_key_equals $temp "stepRange,startStep,endStep" "24 24 24"
# Should this be an error? currently this gets cast from double to int
${tools_dir}/grib_set -s stepRange:d=14.56 $grib2_sample $temp
grib_check_key_equals $temp "stepRange,startStep,endStep" "14 14 14"

# Key validityDateTime
# -----------------------------------------------
input=${data_dir}/constant_field.grib2
grib_check_key_equals $input "dataDate,dataTime,step" "20061205 1200 6"
grib_check_key_equals $input "validityDate,validityTime" "20061205 1800"
grib_check_key_equals $input "validityDateTime:s" "20061205 001800"

# Key julianDay
# -----------------------------------------------
input=${data_dir}/sample.grib2
grib_check_key_equals $input 'julianDay:i' '2454503'
${tools_dir}/grib_set -s julianDay=2454504 $input $temp
grib_check_key_equals $input day 6
grib_check_key_equals $temp day  7

# Seconds (ignored)
# -----------------------------------------------
grib2_sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_ls -s second=9 -n time $grib2_sample 2>$templog
# Something should have been written to stderr
[ -s $templog ]
grep -q "Truncating time: non-zero seconds.* ignored" $templog

# Clean up
rm -f $temp $templog
rm -f $grib2File.p8tmp ${grib2File}.tmp x.grib

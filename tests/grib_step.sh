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

REDIRECT=/dev/null

templog=${data_dir}/log.step.$$
rm -f ${templog} || true
for i in 0 10
do
  for s in 0 1200 600 6000
  do
    for key in stepRange:s startStep endStep
	do
      rm -f ${data_dir}/out.grib | true
      ${tools_dir}/grib_set -s ${key}=$s ${data_dir}/timeRangeIndicator_${i}.grib ${data_dir}/out.grib
#      echo grib_set -s ${key}=$s ${data_dir}/timeRangeIndicator_${i}.grib ${data_dir}/out.grib
#      grib_get -p step,startStep,endStep,P1,P2,timeRangeIndicator,indicatorOfUnitOfTimeRange ${data_dir}/timeRangeIndicator_${i}.grib ${data_dir}/out.grib 
      ${tools_dir}/grib_get -p mars.step,stepRange,startStep,endStep,P1,P2,timeRangeIndicator,indicatorOfUnitOfTimeRange:l ${data_dir}/timeRangeIndicator_${i}.grib ${data_dir}/out.grib >> ${templog}
	done
  done
done

i=5
key=stepRange:s
for s in "0-24" "600-1200" "24-48" "36-66"
do
   ${tools_dir}/grib_set -s ${key}=$s ${data_dir}/timeRangeIndicator_${i}.grib ${data_dir}/out.grib
#   echo grib_set -s ${key}=$s ${data_dir}/timeRangeIndicator_${i}.grib ${data_dir}/out.grib
#   grib_ls -p step,startStep,endStep,P1,P2,timeRangeIndicator,indicatorOfUnitOfTimeRange ${data_dir}/timeRangeIndicator_${i}.grib ${data_dir}/out.grib 
   ${tools_dir}/grib_get -p mars.step,stepRange,startStep,endStep,P1,P2,timeRangeIndicator,indicatorOfUnitOfTimeRange:l ${data_dir}/timeRangeIndicator_${i}.grib ${data_dir}/out.grib >> ${templog}
done

rm -f ${data_dir}/out.grib | true

# test added for ifs stepType=max,min
${tools_dir}/grib_set -s stepType=max,startStep=3,endStep=6 ${data_dir}/reduced_gaussian_model_level.grib1 ${data_dir}/out.grib
${tools_dir}/grib_get -p mars.step,stepRange,startStep,endStep,P1,P2,timeRangeIndicator,indicatorOfUnitOfTimeRange:l ${data_dir}/reduced_gaussian_model_level.grib1 ${data_dir}/out.grib >> ${templog}

rm -f ${data_dir}/out.grib | true

diff ${templog} ${data_dir}/step.log

(${tools_dir}/grib_filter ${data_dir}/step_grib1.filter ${data_dir}/timeRangeIndicator_0.grib > ${templog}) 2>$REDIRECT

diff ${templog} ${data_dir}/step_grib1.log

rm -f ${templog} | true

# GRIB-180
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
${tools_dir}/grib_set -s indicatorOfUnitOfTimeRange=m $grib1_sample $temp
unit=`${tools_dir}/grib_get -p unitOfTimeRange $temp`
[ "$unit" = "0" ]
${tools_dir}/grib_set -s indicatorOfUnitOfTimeRange=m $grib2_sample $temp
unit=`${tools_dir}/grib_get -p indicatorOfUnitOfTimeRange $temp`
[ "$unit" = "0" ]

# ECC-457
input=${data_dir}/tp_ecmwf.grib
stepRange=`${tools_dir}/grib_get -w count=1 -p stepRange,startStep,endStep,stepType $input`
[ "$stepRange" = "12 12 12 instant" ]

rm -f $temp
rm -f $grib2File.p8tmp ${grib2File}.tmp x.grib

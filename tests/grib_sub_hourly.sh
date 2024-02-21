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

# See JIRA issues ECC-1620, ECC-1238
# -----------------------------------

grib_expect_failure() 
{
   a_file=$1
   a_params=$2
   ${tools_dir}/grib_get $a_params $a_file > /dev/null 2>&1
   if [ $? -eq 0 ]; then
      echo "File:     '$a_file'"
      echo "Key(s):   '$a_params'"
      echo "Expected: 'failure'"
      echo "Result:   'success'"
      exit 1
   fi
}

grib_check_key_equals()
{
   a_file=$1
   a_params=$2
   a_expected=$3
   a_result=`${tools_dir}/grib_get $a_params $a_file`
   if [ "$a_result" != "$a_expected" ]; then
      echo "File:     '$a_file'"
      echo "Key(s):   '$a_params'"
      echo "Expected: '$a_expected'"
      echo "Result:   '$a_result'"
      ${tools_dir}/grib_dump -O $a_file | grep -E "indicatorOfUnitOfTimeRange|lengthOfTimeRange|indicatorOfUnitForTimeRange|forecastTime"
      exit 1
   fi
}

HOUR=""
if (set -u; : ${ECCODES_GRIB_HOURLY_STEPS_WITH_UNITS?}) 2> /dev/null; then
   if [ $ECCODES_GRIB_HOURLY_STEPS_WITH_UNITS -gt 0 ]; then
      export HOUR="h"
   fi
fi


label="grib_sub_hourly"
temp=temp.1.$label
temp2=temp.2.$label
tempFilt=temp.$label.filt
tempText=temp.$label.txt

sample_g2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

instantaneous_field=$data_dir/reduced_gaussian_surface.grib2
accumulated_field=$data_dir/reduced_gaussian_sub_area.grib2

# Check the lowercase alias 'stepunits' for a variety of step types (instant, accum etc)
${tools_dir}/grib_get -p stepunits $data_dir/tigge_cf_ecmwf.grib2


#### Make sure that step, stepRange, startStep, endStep produce the same result for instantaneous fields
fn="$instantaneous_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s"
keys_step="step,step:s,step:i,step:d,stepUnits:s"
keys_step_range="stepRange,stepRange:s,stepRange:i,stepRange:d,stepUnits:s"
keys_start_step="startStep,startStep:s,startStep:i,startStep:d,stepUnits:s"
keys_end_step="endStep,endStep:s,endStep:i,endStep:d,stepUnits:s"
${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 h"
${tools_dir}/grib_set -s stepUnits=m,step=59 $fn $temp
grib_check_key_equals $temp "-p $keys_step" "59m 59m 59 59 m"
grib_check_key_equals $temp "-p $keys_step_range" "59m 59m 59 59 m"
grib_check_key_equals $temp "-p $keys_start_step" "59m 59m 59 59 m"
grib_check_key_equals $temp "-p $keys_end_step" "59m 59m 59 59 m"

${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 h"
${tools_dir}/grib_set -s step=59m $fn $temp
grib_check_key_equals $temp "-p $keys_step" "59m 59m 59 59 m"
grib_check_key_equals $temp "-p $keys_step_range" "59m 59m 59 59 m"
grib_check_key_equals $temp "-p $keys_start_step" "59m 59m 59 59 m"
grib_check_key_equals $temp "-p $keys_end_step" "59m 59m 59 59 m"


#### stepUnits overrides the units in the low level keys
# if stepUnits=UNIT is set, then set the low level keys to UNIT
# else optimise low level keys
# instant fields:
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
fn="$instantaneous_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s"
keys__="step,stepUnits:s"
keys_s="step:s"
keys_i="step:i,stepUnits:s"
keys_d="step:d,stepUnits:s"

${tools_dir}/grib_set -s stepUnits=m,step=60 $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "60 m"
grib_check_key_equals $temp "-p $keys_s" "1$HOUR"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=m" "60m"
${tools_dir}/grib_set -s stepUnits=m,step=60 $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "60 m"
grib_check_key_equals $temp "-p $keys_s" "1$HOUR"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=m" "60m"
${tools_dir}/grib_set -s step=60m $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "1 h"
grib_check_key_equals $temp "-p $keys_s" "1$HOUR"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=m" "60m"


# accumulated fields:
fn="$accumulated_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
keys__="step,startStep,endStep,stepRange,stepUnits:s"
keys_s="step:s,startStep:s,endStep:s,stepRange:s,stepUnits:s"
keys_i="step:i,startStep:i,endStep:i,stepRange:i,stepUnits:s"
keys_d="step:d,startStep:d,endStep:d,stepRange:d,stepUnits:s"

${tools_dir}/grib_set -s stepUnits=m,stepRange=60-120 $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "60 m 60 m"
grib_check_key_equals $temp "-p $keys_s" "2$HOUR 1$HOUR 2$HOUR 1$HOUR-2$HOUR h"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=m" "120m 60m 120m 60m-120m m"
${tools_dir}/grib_set -s stepUnits=m,stepRange=60-120 $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "60 m 60 m"
grib_check_key_equals $temp "-p $keys_s" "2$HOUR 1$HOUR 2$HOUR 1$HOUR-2$HOUR h"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=m" "120m 60m 120m 60m-120m m"
${tools_dir}/grib_set -s stepRange=60m-120m $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "1 h 1 h"
grib_check_key_equals $temp "-p $keys_s" "2$HOUR 1$HOUR 2$HOUR 1$HOUR-2$HOUR h"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=m" "120m 60m 120m 60m-120m m"


#### CHECK units
fn="$accumulated_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=96,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 h 96 h"

grib_check_key_equals $temp " -w count=1 -s stepUnits=s -p step:i,stepUnits:s" "345600 s"
grib_check_key_equals $temp " -w count=1 -s stepUnits=m -p step:i,stepUnits:s" "5760 m"
grib_check_key_equals $temp " -w count=1 -s stepUnits=15m -p step:i,stepUnits:s" "384 15m"
grib_check_key_equals $temp " -w count=1 -s stepUnits=30m -p step:i,stepUnits:s" "192 30m"
grib_check_key_equals $temp " -w count=1 -s stepUnits=h -p step:i,stepUnits:s" "96 h"
grib_check_key_equals $temp " -w count=1 -s stepUnits=6h -p step:i,stepUnits:s" "16 6h"
grib_check_key_equals $temp " -w count=1 -s stepUnits=12h -p step:i,stepUnits:s" "8 12h"
grib_check_key_equals $temp " -w count=1 -s stepUnits=D -p step:i,stepUnits:s" "4 D"

grib_check_key_equals $temp " -w count=1 -s stepUnits=s -p step,stepUnits:s" "345600s s"
grib_check_key_equals $temp " -w count=1 -s stepUnits=m -p step,stepUnits:s" "5760m m"
grib_check_key_equals $temp " -w count=1 -s stepUnits=15m -p step,stepUnits:s" "384x15m 15m"
grib_check_key_equals $temp " -w count=1 -s stepUnits=30m -p step,stepUnits:s" "192x30m 30m"
grib_check_key_equals $temp " -w count=1 -s stepUnits=h -p step,stepUnits:s" "96$HOUR h"
grib_check_key_equals $temp " -w count=1 -s stepUnits=6h -p step,stepUnits:s" "16x6h 6h"
grib_check_key_equals $temp " -w count=1 -s stepUnits=12h -p step,stepUnits:s" "8x12h 12h"
grib_check_key_equals $temp " -w count=1 -s stepUnits=D -p step,stepUnits:s" "4D D"

${tools_dir}/grib_set -s stepUnits=s,startStep=0,endStep=345600  $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 s 345600 s"
${tools_dir}/grib_set -s stepUnits=m,startStep=0,endStep=5760  $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 m 5760 m"
${tools_dir}/grib_set -s stepUnits=h,startStep=0,endStep=96  $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 h 96 h"
${tools_dir}/grib_set -s stepUnits=6h,startStep=0,endStep=16  $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 6h 16 6h"
${tools_dir}/grib_set -s stepUnits=12h,startStep=0,endStep=8  $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 12h 8 12h"
${tools_dir}/grib_set -s stepUnits=D,startStep=0,endStep=4  $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 D 4 D"


#### CHECK negative forecastTime
fn="$accumulated_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
${tools_dir}/grib_set -s forecastTime=-6,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=6,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "-6 h 6 h"

grib_check_key_equals $temp "-s stepUnits:s=h -p startStep:s,endStep:s" "-6$HOUR 0$HOUR"
grib_check_key_equals $temp "-s stepUnits:s=m -p startStep:s,endStep:s" "-360m 0m"
grib_check_key_equals $temp "-s stepUnits:s=s -p startStep:s,endStep:s" "-21600s 0s"

${tools_dir}/grib_set -s forecastTime=-48,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=0,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp "-p stepRange" "-48$HOUR"


#### CHECK: check optimal units are set correctly in GRIB files
fn="$accumulated_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "24 h 1 D"

### TODO(maee): @Shahram: how to make parameters position independent
${tools_dir}/grib_set -s stepUnits:s=s,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 s 120 s"
#${tools_dir}/grib_set -s startStep:i=60,endStep:i=180,stepUnits:s=s $temp $temp2
#grib_check_key_equals $temp2   "-p $low_level_keys" "1 m 2 m"

# Seconds
${tools_dir}/grib_set -s stepUnits:i=13,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 s 120 s"
${tools_dir}/grib_set -s stepUnits:s=s,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 s 120 s"

# Minutes
${tools_dir}/grib_set -s stepUnits:i=0,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 m 120 m"
${tools_dir}/grib_set -s stepUnits:s=m,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 m 120 m"

# Hours
${tools_dir}/grib_set -s stepUnits:i=1,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 h 120 h"
${tools_dir}/grib_set -s stepUnits:s=h,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 h 120 h"

#fn="$accumulated_field"
#low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
##high_level_keys="startStep:s,endStep:s"
#high_level_keys="startStep:i,endStep:i"
#${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
#grib_check_key_equals $temp    "-p $low_level_keys" "24 h 1 D"
#grib_check_key_equals $temp   "-p $high_level_keys" "24 48"
#${tools_dir}/grib_set -s startStep:i=24 $temp $temp2
#grib_check_key_equals $temp2   "-p $low_level_keys" "24 h 0 h"
#grib_check_key_equals $temp2   "-p $high_level_keys" "24 24"

#${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=24,indicatorOfUnitForTimeRange=h $fn $temp
#grib_check_key_equals $temp    "-p $low_level_keys" "24 h 24 h"
#grib_check_key_equals $temp   "-p $high_level_keys" "24 48"
#${tools_dir}/grib_set -s startStep:i=24 $temp $temp2
#grib_check_key_equals $temp2   "-p $low_level_keys" "24 h 24 h"
#grib_check_key_equals $temp2   "-p $high_level_keys" "24 48"
#exit

#### CHECK: grib_set - endStep + stepUnits
fn="$accumulated_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "24 h 1 D"

# Use range unit: hour
${tools_dir}/grib_set -s endStep:d=30 $temp $temp2 # TODO(maee) remove in the future behavior
#${tools_dir}/grib_set -s endStep:i=30 $temp $temp2 # TODO(maee) keep for backwards compatibility
#${tools_dir}/grib_set -s endStep:s=30 $temp $temp2 
#${tools_dir}/grib_set -s endStep:s=30h $temp $temp2
#${tools_dir}/grib_set -s endStep=30h $temp $temp2 # TODO(maee) add to tests
grib_check_key_equals $temp2   "-p $low_level_keys" "24 h 6 h"

# Use stepUnits
${tools_dir}/grib_set -s endStep:s=30 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "24 h 6 h"

${tools_dir}/grib_set -s endStep:s=30h $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "24 h 6 h"

${tools_dir}/grib_set -s endStep:s=88200s $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1440 m 30 m"

${tools_dir}/grib_set -s endStep:s=24024 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "24 h 24000 h"

${tools_dir}/grib_set -s startStep:d=5 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "5 h 43 h"

${tools_dir}/grib_set -s startStep:s=5h $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "5 h 43 h"

${tools_dir}/grib_set -s startStep:s=240s $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "4 m 2876 m"

${tools_dir}/grib_set -s startStep:s=2 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "2 h 46 h"

${tools_dir}/grib_set -s stepRange:s=5h-30h $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "5 h 25 h"
grib_check_key_equals $temp2   "-p stepRange:s"     "5$HOUR-30$HOUR"

${tools_dir}/grib_set -s stepRange:s=5-30 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "5 h 25 h"
grib_check_key_equals $temp2   "-p stepRange:s"     "5$HOUR-30$HOUR"

${tools_dir}/grib_set -s stepRange:s=60m-120m $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1 h 1 h"
grib_check_key_equals $temp2   "-p stepRange:s"     "1$HOUR-2$HOUR"

${tools_dir}/grib_set -s stepRange:s=60s-120s $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1 m 1 m"
grib_check_key_equals $temp2   "-p stepRange:s"     "1m-2m"

${tools_dir}/grib_set -s stepRange:s=60m-121m $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 m 61 m"
grib_check_key_equals $temp2   "-p stepRange:s"     "60m-121m"

${tools_dir}/grib_set -s stepRange:s=62D-122D $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1488 h 1440 h"
grib_check_key_equals $temp2   "-p stepRange:s"     "1488$HOUR-2928$HOUR"

fn="$instantaneous_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s"
keys__="step,stepUnits:s"
keys_s="step:s"
keys_i="step:i,stepUnits:s"
keys_d="step:d,stepUnits:s"

${tools_dir}/grib_set -s forecastTime=59,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $keys__ -s stepUnits=s" "3540s s"
grib_check_key_equals $temp "-p $keys__ -s stepUnits=m" "59m m"
#grib_check_key_equals $temp "-p $keys__ -s stepUnits=h" "0" # not supported
grib_check_key_equals $temp "-p $keys_s -s stepUnits=s" "3540s"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=m" "59m"
#grib_check_key_equals $temp "-p $keys_s -F"%.2f" -s stepUnits=h" "0.98" # not supported
grib_check_key_equals $temp "-p $keys_i -s stepUnits=s" "3540 s"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=m" "59 m"
#grib_check_key_equals $temp "-p $keys_i -s stepUnits=h" "0" # not supported
grib_check_key_equals $temp "-p $keys_d -s stepUnits=s" "3540 s"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=m" "59 m"
#grib_check_key_equals $temp "-p $keys_d -s stepUnits=h" "0.983333" # not supported


${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $keys_i -s stepUnits=s" "0 s"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=m" "0 m"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=h" "0 h"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=s" "0 s"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=m" "0 m"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=h" "0 h"


fn="$instantaneous_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s"
keys__="step,stepUnits:s"
keys_s="step:s,stepUnits:s"
keys_i="step:i,stepUnits:s"
keys_d="step:d,stepUnits:s"

${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 m"
grib_check_key_equals $temp "-p $keys__" "0m m"
grib_check_key_equals $temp "-p $keys_s" "0m m"
grib_check_key_equals $temp "-p $keys_i" "0 m"
grib_check_key_equals $temp "-p $keys_d" "0 m"

grib_check_key_equals $temp "-p $keys__ -s stepUnits=s" "0s s"
grib_check_key_equals $temp "-p $keys__ -s stepUnits=m" "0m m"
grib_check_key_equals $temp "-p $keys__ -s stepUnits=h" "0$HOUR h"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=s" "0s s"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=m" "0m m"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=h" "0$HOUR h"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=s" "0 s"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=m" "0 m"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=h" "0 h"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=s" "0 s"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=m" "0 m"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=h" "0 h"


${tools_dir}/grib_set -s forecastTime=59,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "59 m"
grib_check_key_equals $temp "-p $keys__" "59m m"
#grib_check_key_equals $temp    "-p $keys_s" "59"
grib_check_key_equals $temp "-p $keys_s" "59m m"
grib_check_key_equals $temp "-p $keys_i" "59 m"
grib_check_key_equals $temp "-p $keys_d" "59 m"

${tools_dir}/grib_set -s forecastTime=60,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "60 m"
grib_check_key_equals $temp "-p $keys__" "1$HOUR h"
grib_check_key_equals $temp "-p $keys_s" "1$HOUR h"
grib_check_key_equals $temp "-p $keys_i" "1 h"
grib_check_key_equals $temp "-p $keys_d" "1 h"

${tools_dir}/grib_set -s forecastTime=61,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "61 m"
grib_check_key_equals $temp "-p $keys__" "61m m"
#grib_check_key_equals $temp    "-p $keys_s" "61"
grib_check_key_equals $temp "-p $keys_s" "61m m"
grib_check_key_equals $temp "-p $keys_i" "61 m"
grib_check_key_equals $temp "-p $keys_d" "61 m"

${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "24 h"
grib_check_key_equals $temp "-p $keys__" "24$HOUR h"
grib_check_key_equals $temp "-p $keys_s" "24$HOUR h"
grib_check_key_equals $temp "-p $keys_i" "24 h"
grib_check_key_equals $temp "-p $keys_d" "24 h"

${tools_dir}/grib_set -s forecastTime=1440,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "1440 m"
grib_check_key_equals $temp "-p $keys__" "24$HOUR h"
grib_check_key_equals $temp "-p $keys_s" "24$HOUR h"
grib_check_key_equals $temp "-p $keys_i" "24 h"
grib_check_key_equals $temp "-p $keys_d" "24 h"


fn="$accumulated_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
${tools_dir}/grib_set -s stepRange=60m-2h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "1 h 1 h"

fn="$accumulated_field"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
keys__="stepRange,startStep,endStep"
keys_s="stepRange:s,startStep:s,endStep:s"
keys_i="stepRange:i,startStep:i,endStep:i"
keys_d="stepRange:d,startStep:d,endStep:d"

${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=2,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "0 m 2 h"
grib_check_key_equals $temp "-p $keys__" "0$HOUR-2$HOUR 0$HOUR 2$HOUR"
grib_check_key_equals $temp "-p $keys_s" "0$HOUR-2$HOUR 0$HOUR 2$HOUR"
grib_check_key_equals $temp "-p $keys_i" "2 0 2"
grib_check_key_equals $temp "-p $keys_d" "2 0 2"

${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "24 h 1 D"
grib_check_key_equals $temp "-p $keys__" "24$HOUR-48$HOUR 24$HOUR 48$HOUR"
grib_check_key_equals $temp "-p $keys_s" "24$HOUR-48$HOUR 24$HOUR 48$HOUR"
grib_check_key_equals $temp "-p $keys_i" "48 24 48" 
grib_check_key_equals $temp "-p $keys_d" "48 24 48" 

${tools_dir}/grib_set -s forecastTime=25,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "25 h 1 D"
grib_check_key_equals $temp "-p $keys__" "25$HOUR-49$HOUR 25$HOUR 49$HOUR"
grib_check_key_equals $temp "-p $keys_s" "25$HOUR-49$HOUR 25$HOUR 49$HOUR"
grib_check_key_equals $temp "-p $keys_i" "49 25 49" 
grib_check_key_equals $temp "-p $keys_d" "49 25 49" 

${tools_dir}/grib_set -s forecastTime=45,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=15,indicatorOfUnitForTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "45 m 15 m"
grib_check_key_equals $temp "-p $keys__" "45m-60m 45m 60m" 
#grib_check_key_equals $temp    "-p $keys_s" "45-60 45 60"  
grib_check_key_equals $temp "-p $keys_s" "45m-60m 45m 60m"  
grib_check_key_equals $temp "-p $keys_i" "60 45 60"  
grib_check_key_equals $temp "-p $keys_d" "60 45 60"

${tools_dir}/grib_set -s forecastTime=60,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=2,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "60 m 2 h"
grib_check_key_equals $temp "-p $keys__" "1$HOUR-3$HOUR 1$HOUR 3$HOUR" 
grib_check_key_equals $temp "-p $keys_s" "1$HOUR-3$HOUR 1$HOUR 3$HOUR"
grib_check_key_equals $temp "-p $keys_i" "3 1 3"
grib_check_key_equals $temp "-p $keys_d" "3 1 3"

${tools_dir}/grib_set -s forecastTime=18,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=6,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "18 h 6 h"
grib_check_key_equals $temp "-p $keys__" "18$HOUR-24$HOUR 18$HOUR 24$HOUR"
grib_check_key_equals $temp "-p $keys_s" "18$HOUR-24$HOUR 18$HOUR 24$HOUR"
grib_check_key_equals $temp "-p $keys_i" "24 18 24"
grib_check_key_equals $temp "-p $keys_d" "24 18 24"

${tools_dir}/grib_set -s forecastTime=1080,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=360,indicatorOfUnitForTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "1080 m 360 m"
grib_check_key_equals $temp "-p $keys__" "18$HOUR-24$HOUR 18$HOUR 24$HOUR"
grib_check_key_equals $temp "-p $keys_s" "18$HOUR-24$HOUR 18$HOUR 24$HOUR"
grib_check_key_equals $temp "-p $keys_i" "24 18 24"
grib_check_key_equals $temp "-p $keys_d" "24 18 24"

${tools_dir}/grib_set -s forecastTime=1080,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=6,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "1080 m 6 h"
grib_check_key_equals $temp "-p $keys__" "18$HOUR-24$HOUR 18$HOUR 24$HOUR"
grib_check_key_equals $temp "-p $keys_s" "18$HOUR-24$HOUR 18$HOUR 24$HOUR"
grib_check_key_equals $temp "-p $keys_i" "24 18 24"
grib_check_key_equals $temp "-p $keys_d" "24 18 24"

# Pack expression
cat >$tempFilt<<EOF
    set step="16m"; write;
EOF
${tools_dir}/grib_filter -o $temp $tempFilt $sample_g2
grib_check_key_equals $temp "-p indicatorOfUnitOfTimeRange,forecastTime" "0 16"

cat >$tempFilt<<EOF
    set stepUnits="s"; set step="16"; write;
EOF
${tools_dir}/grib_filter -o $temp $tempFilt $sample_g2
grib_check_key_equals $temp "-p indicatorOfUnitOfTimeRange,forecastTime" "13 16"


cat >$tempFilt<<EOF
    set stepUnits="s"; set step=16; write;
EOF
${tools_dir}/grib_filter -o $temp $tempFilt $sample_g2
grib_check_key_equals $temp "-p indicatorOfUnitOfTimeRange,forecastTime" "13 16"


cat >$tempFilt<<EOF
    set stepUnits="m"; print "[step]";
EOF
${tools_dir}/grib_filter $tempFilt $data_dir/constant_field.grib2


cat >$tempFilt<<EOF
    set stepUnits="s"; print "[step]";
EOF
${tools_dir}/grib_filter $tempFilt $data_dir/constant_field.grib2

# Setting stepUnits as integer via filter
cat >$tempFilt<<EOF
    set stepUnits = 0;
    set startStep = "16";
    write;
EOF
${tools_dir}/grib_filter -o $temp $tempFilt $sample_g2
grib_check_key_equals $temp '-p startStep' '16m'
grib_check_key_equals $temp '-p indicatorOfUnitOfTimeRange' '0'
grib_check_key_equals $temp '-p forecastTime' '16'


export ECCODES_GRIB_HOURLY_STEPS_WITH_UNITS=1
cat >$tempFilt<<EOF
    assert ( step is "6h" );
EOF
cat $tempFilt
${tools_dir}/grib_filter $tempFilt $data_dir/constant_field.grib2
unset ECCODES_GRIB_HOURLY_STEPS_WITH_UNITS


# Changing the product definition template
# ----------------------------------------
# See ECC-1768
${tools_dir}/grib_set -s step=62m $sample_g2 $temp
${tools_dir}/grib_set -s productDefinitionTemplateNumber=8 $temp $temp2

${tools_dir}/grib_set -s productDefinitionTemplateNumber=8,stepUnits=s,step=0 $sample_g2 $temp
grib_check_key_equals $temp '-p stepUnits:s,startStep,productDefinitionTemplateNumber' 's 0s 8'

${tools_dir}/grib_set -s productDefinitionTemplateNumber=8,stepUnits=m,step=60 $sample_g2 $temp
grib_check_key_equals $temp '-p stepUnits:s,productDefinitionTemplateNumber' 'h 8'


# Bad stepUnits
set +e
${tools_dir}/grib_set -s stepUnits=190 $sample_g2 $temp > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid unit" $tempText

# Clean up
rm -f $temp $temp2 $tempFilt $tempText

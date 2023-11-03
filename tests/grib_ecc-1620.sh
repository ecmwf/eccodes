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

label="grib_ecc-1620"
temp=temp.$label
temp2=temp_2.$label
samples_dir=$ECCODES_SAMPLES_PATH


#### CHECK negative forecastTime
fn="${data_dir}/reduced_gaussian_sub_area.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
${tools_dir}/grib_set -s forecastTime=-6,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=6,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "-6 h 6 h"

grib_check_key_equals $temp "-s stepUnits:s=h -p startStep:s,endStep:s" "-6 0"
grib_check_key_equals $temp "-s stepUnits:s=m -p startStep:s,endStep:s" "-360m 0m"
grib_check_key_equals $temp "-s stepUnits:s=s -p startStep:s,endStep:s" "-21600s 0s"

${tools_dir}/grib_set -s forecastTime=-48,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=0,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp "-p stepRange" "-48"


#### CHECK: check optimal units are set correctly in GRIB files
fn="${data_dir}/reduced_gaussian_sub_area.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "24 h 1 D"

### TODO(EB): @Shahram: how to make parameters position independent
${tools_dir}/grib_set -s stepUnits:s=s,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1 m 2 m"
#${tools_dir}/grib_set -s startStep:i=60,endStep:i=180,stepUnits:s=s $temp $temp2
#grib_check_key_equals $temp2   "-p $low_level_keys" "1 m 2 m"

${tools_dir}/grib_set -s stepUnits:i=13,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1 m 2 m"
${tools_dir}/grib_set -s stepUnits:s=s,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1 m 2 m"

${tools_dir}/grib_set -s stepUnits:i=0,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1 h 2 h"
${tools_dir}/grib_set -s stepUnits:s=m,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1 h 2 h"

${tools_dir}/grib_set -s stepUnits:i=1,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 h 120 h"
${tools_dir}/grib_set -s stepUnits:s=h,startStep:i=60,endStep:i=180 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 h 120 h"

#fn="${data_dir}/reduced_gaussian_sub_area.grib2"
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
fn="${data_dir}/reduced_gaussian_sub_area.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "24 h 1 D"

# Use range unit: hour
${tools_dir}/grib_set -s endStep:d=30 $temp $temp2 # TODO(EB) remove in the future behavior
#${tools_dir}/grib_set -s endStep:i=30 $temp $temp2 # TODO(EB) keep for backwards compatibility
#${tools_dir}/grib_set -s endStep:s=30 $temp $temp2 
#${tools_dir}/grib_set -s endStep:s=30h $temp $temp2
#${tools_dir}/grib_set -s endStep=30h $temp $temp2 # TODO(EB) add to tests
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
grib_check_key_equals $temp2   "-p stepRange:s"     "5-30"

${tools_dir}/grib_set -s stepRange:s=5-30 $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "5 h 25 h"
grib_check_key_equals $temp2   "-p stepRange:s"     "5-30"

${tools_dir}/grib_set -s stepRange:s=60m-120m $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1 h 1 h"
grib_check_key_equals $temp2   "-p stepRange:s"     "1-2"

${tools_dir}/grib_set -s stepRange:s=60s-120s $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1 m 1 m"
grib_check_key_equals $temp2   "-p stepRange:s"     "1m-2m"

${tools_dir}/grib_set -s stepRange:s=60m-121m $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "60 m 61 m"
grib_check_key_equals $temp2   "-p stepRange:s"     "60m-121m"

${tools_dir}/grib_set -s stepRange:s=62D-122D $temp $temp2
grib_check_key_equals $temp2   "-p $low_level_keys" "1488 h 1440 h"
grib_check_key_equals $temp2   "-p stepRange:s"     "1488-2928"

fn="${data_dir}/reduced_gaussian_surface.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s"
keys__="step,stepUnits:s"
keys_s="step:s"
keys_i="step:i,stepUnits:s"
keys_d="step:d,stepUnits:s"

${tools_dir}/grib_set -s forecastTime=59,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $keys__ -s stepUnits=s" "3540 s"
grib_check_key_equals $temp "-p $keys__ -s stepUnits=m" "59 m"
#grib_check_key_equals $temp "-p $keys__ -s stepUnits=h" "0" # TODO(EB): check behaviour (should be 0.983333)
grib_check_key_equals $temp "-p $keys_s -s stepUnits=s" "3540s"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=m" "59m"
#grib_check_key_equals $temp "-p $keys_s -F"%.2f" -s stepUnits=h" "0.983333" # TODO(EB): check behaviour // See tools for default output format
grib_check_key_equals $temp "-p $keys_i -s stepUnits=s" "3540 s"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=m" "59 m"
#grib_check_key_equals $temp "-p $keys_i -s stepUnits=h" "0" # TODO(EB): check behaviour
grib_check_key_equals $temp "-p $keys_d -s stepUnits=s" "3540 s"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=m" "59 m"
#grib_check_key_equals $temp "-p $keys_d -s stepUnits=h" "0.983333" # TODO(EB): check behaviour


${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-p $keys_i -s stepUnits=s" "0 s"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=m" "0 m"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=h" "0 h"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=s" "0 s"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=m" "0 m"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=h" "0 h"


fn="${data_dir}/reduced_gaussian_surface.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s"
keys__="step,stepUnits:s"
keys_s="step:s,stepUnits:s"
keys_i="step:i,stepUnits:s"
keys_d="step:d,stepUnits:s"

${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "0 m"
grib_check_key_equals $temp "-p $keys__" "0 m"
grib_check_key_equals $temp "-p $keys_s" "0m m"
grib_check_key_equals $temp "-p $keys_i" "0 m"
grib_check_key_equals $temp "-p $keys_d" "0 m"

grib_check_key_equals $temp "-p $keys__ -s stepUnits=s" "0 s"
grib_check_key_equals $temp "-p $keys__ -s stepUnits=m" "0 m"
grib_check_key_equals $temp "-p $keys__ -s stepUnits=h" "0 h"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=s" "0s s"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=m" "0m m"
grib_check_key_equals $temp "-p $keys_s -s stepUnits=h" "0 h"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=s" "0 s"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=m" "0 m"
grib_check_key_equals $temp "-p $keys_i -s stepUnits=h" "0 h"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=s" "0 s"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=m" "0 m"
grib_check_key_equals $temp "-p $keys_d -s stepUnits=h" "0 h"


${tools_dir}/grib_set -s forecastTime=59,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "59 m"
grib_check_key_equals $temp "-p $keys__" "59 m"
#grib_check_key_equals $temp    "-p $keys_s" "59"
grib_check_key_equals $temp "-p $keys_s" "59m m"
grib_check_key_equals $temp "-p $keys_i" "59 m"
grib_check_key_equals $temp "-p $keys_d" "59 m"

${tools_dir}/grib_set -s forecastTime=60,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "60 m"
grib_check_key_equals $temp "-p $keys__" "1 h"
grib_check_key_equals $temp "-p $keys_s" "1 h"
grib_check_key_equals $temp "-p $keys_i" "1 h"
grib_check_key_equals $temp "-p $keys_d" "1 h"

${tools_dir}/grib_set -s forecastTime=61,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "61 m"
grib_check_key_equals $temp "-p $keys__" "61 m"
#grib_check_key_equals $temp    "-p $keys_s" "61"
grib_check_key_equals $temp "-p $keys_s" "61m m"
grib_check_key_equals $temp "-p $keys_i" "61 m"
grib_check_key_equals $temp "-p $keys_d" "61 m"

${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "24 h"
grib_check_key_equals $temp "-p $keys__" "24 h"
grib_check_key_equals $temp "-p $keys_s" "24 h"
grib_check_key_equals $temp "-p $keys_i" "24 h"
grib_check_key_equals $temp "-p $keys_d" "24 h"

${tools_dir}/grib_set -s forecastTime=1440,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "1440 m"
grib_check_key_equals $temp "-p $keys__" "24 h"
grib_check_key_equals $temp "-p $keys_s" "24 h"
grib_check_key_equals $temp "-p $keys_i" "24 h"
grib_check_key_equals $temp "-p $keys_d" "24 h"


fn="${data_dir}/reduced_gaussian_sub_area.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
${tools_dir}/grib_set -s stepRange=60m-2h $fn $temp
grib_check_key_equals $temp "-p $low_level_keys" "1 h 1 h"


fn="${data_dir}/reduced_gaussian_sub_area.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
keys__="stepRange,startStep,endStep"
keys_s="stepRange:s,startStep:s,endStep:s"
keys_i="stepRange:i,startStep:i,endStep:i"
keys_d="stepRange:d,startStep:d,endStep:d"

${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=2,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "0 m 2 h"
grib_check_key_equals $temp "-p $keys__" "0-2 0 2"
grib_check_key_equals $temp "-p $keys_s" "0-2 0 2"
grib_check_key_equals $temp "-p $keys_i" "2 0 2"
grib_check_key_equals $temp "-p $keys_d" "2 0 2"

${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "24 h 1 D"
grib_check_key_equals $temp "-p $keys__" "24-48 24 48"
grib_check_key_equals $temp "-p $keys_s" "24-48 24 48"
grib_check_key_equals $temp "-p $keys_i" "48 24 48" 
grib_check_key_equals $temp "-p $keys_d" "48 24 48" 

${tools_dir}/grib_set -s forecastTime=25,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "25 h 1 D"
grib_check_key_equals $temp    "-p $keys__" "25-49 25 49"
grib_check_key_equals $temp "-p $keys__" "25-49 25 49"
grib_check_key_equals $temp "-p $keys_s" "25-49 25 49"
grib_check_key_equals $temp "-p $keys_i" "49 25 49" 
grib_check_key_equals $temp "-p $keys_d" "49 25 49" 

${tools_dir}/grib_set -s forecastTime=45,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=15,indicatorOfUnitForTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "45 m 15 m"
grib_check_key_equals $temp "-p $keys__" "45m-60m 45 60" 
#grib_check_key_equals $temp    "-p $keys_s" "45-60 45 60"  
grib_check_key_equals $temp "-p $keys_s" "45m-60m 45m 60m"  
grib_check_key_equals $temp "-p $keys_i" "60 45 60"  
grib_check_key_equals $temp "-p $keys_d" "60 45 60"

${tools_dir}/grib_set -s forecastTime=60,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=2,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "60 m 2 h"
grib_check_key_equals $temp "-p $keys__" "1-3 1 3" 
grib_check_key_equals $temp "-p $keys_s" "1-3 1 3"
grib_check_key_equals $temp "-p $keys_i" "3 1 3"
grib_check_key_equals $temp "-p $keys_d" "3 1 3"

${tools_dir}/grib_set -s forecastTime=18,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=6,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "18 h 6 h"
grib_check_key_equals $temp "-p $keys__" "18-24 18 24"
grib_check_key_equals $temp "-p $keys_s" "18-24 18 24"
grib_check_key_equals $temp "-p $keys_i" "24 18 24"
grib_check_key_equals $temp "-p $keys_d" "24 18 24"

${tools_dir}/grib_set -s forecastTime=1080,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=360,indicatorOfUnitForTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "1080 m 360 m"
grib_check_key_equals $temp "-p $keys__" "18-24 18 24"
grib_check_key_equals $temp "-p $keys_s" "18-24 18 24"
grib_check_key_equals $temp "-p $keys_i" "24 18 24"
grib_check_key_equals $temp "-p $keys_d" "24 18 24"

${tools_dir}/grib_set -s forecastTime=1080,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=6,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "1080 m 6 h"
grib_check_key_equals $temp "-p $keys__" "18-24 18 24"
grib_check_key_equals $temp "-p $keys_s" "18-24 18 24"
grib_check_key_equals $temp "-p $keys_i" "24 18 24"
grib_check_key_equals $temp "-p $keys_d" "24 18 24"

rm -f $temp $temp2

#~/build/eccodes/bin/grib_ls -m /perm/maro/referenceGRIBfiles4MTG2testing/grib1+2_operational_and_rd/151145_s2_enfo_cf_o2d_zos_2002_prod_ecmf_glob.grib2
#~/build/eccodes/bin/grib_ls -m /perm/maro/referenceGRIBfiles4MTG2testing/grib1+2_operational_and_rd/240023_ce_efas_fc_sfc_dis06_2022_0001_ecmf_lisflood.grib2

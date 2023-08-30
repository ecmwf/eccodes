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



#### CHECK: grib_set - endStep + stepUnits
fn="${data_dir}/reduced_gaussian_sub_area.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "24 h 1 D"

# Use range unit: hour
${tools_dir}/grib_set -y -s endStep:d=30 $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "24 h 6 h"
#${tools_dir}/grib_set -y -s endStep:d=24.5 $temp $temp2
#grib_check_key_equals $temp2   "-y -p $low_level_keys" "24 h 30 m"

# Use stepUnits
${tools_dir}/grib_set -y -s endStep:s=30h $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "24 h 6 h"
#${tools_dir}/grib_set -y -s endStep:s=24.5h $temp $temp2
#grib_check_key_equals $temp2   "-y -p $low_level_keys" "24 h 30 m"
${tools_dir}/grib_set -y -s endStep:s=88200s $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "24 h 30 m"
#${tools_dir}/grib_set -y -s endStep:s=1446.65m $temp $temp2
#grib_check_key_equals $temp2   "-y -p $low_level_keys" "24 h 399 s"
${tools_dir}/grib_set -y -s endStep:s=24024 $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "24 h 1000 D"

# Use range unit: hour
${tools_dir}/grib_set -y -s startStep:d=5 $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "5 h 1 D"
#${tools_dir}/grib_set -y -s startStep:d=4.5 $temp $temp2
#grib_check_key_equals $temp2   "-y -p $low_level_keys" "270 m 1 D"

# Use stepUnits
${tools_dir}/grib_set -y -s startStep:s=5h $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "5 h 1 D"
#${tools_dir}/grib_set -y -s startStep:s=4.5h $temp $temp2
#grib_check_key_equals $temp2   "-y -p $low_level_keys" "270 m 1 D"
${tools_dir}/grib_set -y -s startStep:s=240s $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "4 m 1 D"
#${tools_dir}/grib_set -y -s startStep:s=0.65m $temp $temp2
#grib_check_key_equals $temp2   "-y -p $low_level_keys" "39 s 1 D"
${tools_dir}/grib_set -y -s startStep:s=2 $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "2 h 1 D"





${tools_dir}/grib_set -y -s stepRange:s=5h-30h $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "5 h 25 h"
grib_check_key_equals $temp2   "-y -p stepRange:s"     "5-30"

${tools_dir}/grib_set -y -s stepRange:s=5-30 $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "5 h 25 h"
grib_check_key_equals $temp2   "-y -p stepRange:s"     "5-30"

${tools_dir}/grib_set -y -s stepRange:s=60m-120m $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "1 h 1 h"
grib_check_key_equals $temp2   "-y -p stepRange:s"     "1-2"

${tools_dir}/grib_set -y -s stepRange:s=60s-120s $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "1 m 1 m"
grib_check_key_equals $temp2   "-y -p stepRange:s"     "1m-2m"

${tools_dir}/grib_set -y -s stepRange:s=60m-121m $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "1 h 61 m"
grib_check_key_equals $temp2   "-y -p stepRange:s"     "60m-121m"

${tools_dir}/grib_set -y -s stepRange:s=62D-122D $temp $temp2
grib_check_key_equals $temp2   "-y -p $low_level_keys" "1488 h 60 D"
grib_check_key_equals $temp2   "-y -p stepRange:s"     "1488-2928"

#${tools_dir}/grib_set -s  $fn $temp

#fn="${data_dir}/reduced_gaussian_surface.grib2"
#low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s"
#keys__="step"
#keys_s="step:s"
#keys_i="step:i"
#keys_d="step:d"


#${tools_dir}/grib_set -s forecastTime=59,indicatorOfUnitOfTimeRange=m $fn $temp
#${tools_dir}/grib_set -s step:d=10 $fn $temp
#grib_check_key_equals $temp "-y -p $keys_s -s stepUnits=s" "10m"
#exit



fn="${data_dir}/reduced_gaussian_surface.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s"
keys__="step"
keys_s="step:s"
keys_i="step:i"
keys_d="step:d"


${tools_dir}/grib_set -s forecastTime=59,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-y -p $keys__ -s stepUnits=s" "3540"
grib_check_key_equals $temp "-y -p $keys__ -s stepUnits=m" "59"
#grib_check_key_equals $temp "-y -p $keys__ -s stepUnits=h" "0" # TODO(EB): check behaviour (should be 0.983333)
grib_check_key_equals $temp "-y -p $keys_s -s stepUnits=s" "3540s"
grib_check_key_equals $temp "-y -p $keys_s -s stepUnits=m" "59m"
#grib_check_key_equals $temp "-y -p $keys_s -s stepUnits=h" "0.983333" # TODO(EB): check behaviour
grib_check_key_equals $temp "-y -p $keys_i -s stepUnits=s" "3540"
grib_check_key_equals $temp "-y -p $keys_i -s stepUnits=m" "59"
#grib_check_key_equals $temp "-y -p $keys_i -s stepUnits=h" "0" # TODO(EB): check behaviour
grib_check_key_equals $temp "-y -p $keys_d -s stepUnits=s" "3540"
grib_check_key_equals $temp "-y -p $keys_d -s stepUnits=m" "59"
#grib_check_key_equals $temp "-y -p $keys_d -s stepUnits=h" "0.983333" # TODO(EB): check behaviour


${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp "-y -p $keys_i -s stepUnits=s" "0"
grib_check_key_equals $temp "-y -p $keys_i -s stepUnits=m" "0"
grib_check_key_equals $temp "-y -p $keys_i -s stepUnits=h" "0"
grib_check_key_equals $temp "-y -p $keys_d -s stepUnits=s" "0"
grib_check_key_equals $temp "-y -p $keys_d -s stepUnits=m" "0"
grib_check_key_equals $temp "-y -p $keys_d -s stepUnits=h" "0"



fn="${data_dir}/reduced_gaussian_surface.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s"
keys__="step"
keys_s="step:s"
keys_i="step:i"
keys_d="step:d"


${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "0 m"
grib_check_key_equals $temp    "-p $keys__" "0"
grib_check_key_equals $temp "-y -p $keys__" "0"
grib_check_key_equals $temp    "-p $keys_s" "0"
grib_check_key_equals $temp "-y -p $keys_s" "0"
grib_check_key_equals $temp    "-p $keys_i" "0"
grib_check_key_equals $temp "-y -p $keys_i" "0"
grib_check_key_equals $temp    "-p $keys_d" "0"
grib_check_key_equals $temp "-y -p $keys_d" "0"

grib_check_key_equals $temp "-y -p $keys__ -s stepUnits=s" "0"
grib_check_key_equals $temp "-y -p $keys__ -s stepUnits=m" "0"
grib_check_key_equals $temp "-y -p $keys__ -s stepUnits=h" "0"
grib_check_key_equals $temp "-y -p $keys_s -s stepUnits=s" "0s"
grib_check_key_equals $temp "-y -p $keys_s -s stepUnits=m" "0m"
grib_check_key_equals $temp "-y -p $keys_s -s stepUnits=h" "0"
grib_check_key_equals $temp "-y -p $keys_i -s stepUnits=s" "0"
grib_check_key_equals $temp "-y -p $keys_i -s stepUnits=m" "0"
grib_check_key_equals $temp "-y -p $keys_i -s stepUnits=h" "0"
grib_check_key_equals $temp "-y -p $keys_d -s stepUnits=s" "0"
grib_check_key_equals $temp "-y -p $keys_d -s stepUnits=m" "0"
grib_check_key_equals $temp "-y -p $keys_d -s stepUnits=h" "0"


${tools_dir}/grib_set -s forecastTime=59,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "59 m"
grib_check_key_equals $temp    "-p $keys__" "59"
grib_check_key_equals $temp "-y -p $keys__" "59"
grib_check_key_equals $temp    "-p $keys_s" "59"
grib_check_key_equals $temp "-y -p $keys_s" "59m"
grib_check_key_equals $temp    "-p $keys_i" "59"
grib_check_key_equals $temp "-y -p $keys_i" "59"
grib_check_key_equals $temp    "-p $keys_d" "59"
grib_check_key_equals $temp "-y -p $keys_d" "59"

${tools_dir}/grib_set -s forecastTime=60,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "60 m"
grib_check_key_equals $temp    "-p $keys__" "1"
grib_check_key_equals $temp "-y -p $keys__" "1"
grib_check_key_equals $temp    "-p $keys_s" "1"
grib_check_key_equals $temp "-y -p $keys_s" "1"
grib_check_key_equals $temp    "-p $keys_i" "1"
grib_check_key_equals $temp "-y -p $keys_i" "1"
grib_check_key_equals $temp    "-p $keys_d" "1"
grib_check_key_equals $temp "-y -p $keys_d" "1"

${tools_dir}/grib_set -s forecastTime=61,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "61 m"
grib_check_key_equals $temp    "-p $keys__" "61"
grib_check_key_equals $temp "-y -p $keys__" "61"
grib_check_key_equals $temp    "-p $keys_s" "61"
grib_check_key_equals $temp "-y -p $keys_s" "61m"
grib_check_key_equals $temp    "-p $keys_i" "61"
grib_check_key_equals $temp "-y -p $keys_i" "61"
grib_check_key_equals $temp    "-p $keys_d" "61"
grib_check_key_equals $temp "-y -p $keys_d" "61"

${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "24 h"
grib_check_key_equals $temp    "-p $keys__" "24"
grib_check_key_equals $temp "-y -p $keys__" "24"
grib_check_key_equals $temp    "-p $keys_s" "24"
grib_check_key_equals $temp "-y -p $keys_s" "24"
grib_check_key_equals $temp    "-p $keys_i" "24"
grib_check_key_equals $temp "-y -p $keys_i" "24"
grib_check_key_equals $temp    "-p $keys_d" "24"
grib_check_key_equals $temp "-y -p $keys_d" "24"

${tools_dir}/grib_set -s forecastTime=1440,indicatorOfUnitOfTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "1440 m"
grib_check_key_equals $temp    "-p $keys__" "24"
grib_check_key_equals $temp "-y -p $keys__" "24"
grib_check_key_equals $temp    "-p $keys_s" "24"
grib_check_key_equals $temp "-y -p $keys_s" "24"
grib_check_key_equals $temp    "-p $keys_i" "24"
grib_check_key_equals $temp "-y -p $keys_i" "24"
grib_check_key_equals $temp    "-p $keys_d" "24"
grib_check_key_equals $temp "-y -p $keys_d" "24"



fn="${data_dir}/reduced_gaussian_sub_area.grib2"
low_level_keys="forecastTime,indicatorOfUnitOfTimeRange:s,lengthOfTimeRange,indicatorOfUnitForTimeRange:s"
keys__="stepRange,startStep,endStep"
keys_s="stepRange:s,startStep:s,endStep:s"
keys_i="stepRange:i,startStep:i,endStep:i"
keys_d="stepRange:d,startStep:d,endStep:d"

${tools_dir}/grib_set -s forecastTime=0,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=2,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "0 m 2 h"
grib_check_key_equals $temp    "-p $keys__" "0-2 0 2"
grib_check_key_equals $temp "-y -p $keys__" "0-2 0 2"
grib_check_key_equals $temp    "-p $keys_s" "0-2 0 2"
grib_check_key_equals $temp "-y -p $keys_s" "0-2 0 2"
grib_check_key_equals $temp    "-p $keys_i" "2 0 2"
grib_check_key_equals $temp "-y -p $keys_i" "2 0 2"
grib_check_key_equals $temp    "-p $keys_d" "2 0 2"
grib_check_key_equals $temp "-y -p $keys_d" "2 0 2"

${tools_dir}/grib_set -s forecastTime=24,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "24 h 1 D"
grib_check_key_equals $temp    "-p $keys__" "24-48 24 48"
grib_check_key_equals $temp "-y -p $keys__" "24-48 24 48"
grib_check_key_equals $temp    "-p $keys_s" "24-48 24 48"
grib_check_key_equals $temp "-y -p $keys_s" "24-48 24 48"
grib_check_key_equals $temp    "-p $keys_i" "48 24 48" 
grib_check_key_equals $temp "-y -p $keys_i" "48 24 48" 
grib_check_key_equals $temp    "-p $keys_d" "48 24 48" 
grib_check_key_equals $temp "-y -p $keys_d" "48 24 48" 

${tools_dir}/grib_set -s forecastTime=25,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=1,indicatorOfUnitForTimeRange=D $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "25 h 1 D"
grib_check_key_equals $temp    "-p $keys__" "25-49 25 49"
grib_check_key_equals $temp "-y -p $keys__" "25-49 25 49"
grib_check_key_equals $temp    "-p $keys_s" "25-49 25 49"
grib_check_key_equals $temp "-y -p $keys_s" "25-49 25 49"
grib_check_key_equals $temp    "-p $keys_i" "49 25 49" 
grib_check_key_equals $temp "-y -p $keys_i" "49 25 49" 
grib_check_key_equals $temp    "-p $keys_d" "49 25 49" 
grib_check_key_equals $temp "-y -p $keys_d" "49 25 49" 


#grib_check_key_equals $temp "-y -s stepUnits=h -p $keys__" "1.0166666666666666"
#grib_check_key_equals $temp "-y -s stepUnits=h -p $keys_s" "1.0166666666666666"
#grib_check_key_equals $temp "-y -s stepUnits=h -p $keys_i" "1"
#grib_check_key_equals $temp "-y -s stepUnits=h -p $keys_d" "1.0166666666666666"



${tools_dir}/grib_set -s forecastTime=45,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=15,indicatorOfUnitForTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "45 m 15 m"
grib_check_key_equals $temp    "-p $keys__" "45-60 45 60" 
grib_check_key_equals $temp "-y -p $keys__" "45m-60m 45 60" 
grib_check_key_equals $temp    "-p $keys_s" "45-60 45 60"  
grib_check_key_equals $temp "-y -p $keys_s" "45m-60m 45m 60m"  
grib_check_key_equals $temp    "-p $keys_i" "60 45 60"  
grib_check_key_equals $temp "-y -p $keys_i" "60 45 60"  
grib_check_key_equals $temp    "-p $keys_d" "60 45 60"  
grib_check_key_equals $temp "-y -p $keys_d" "60 45 60"


${tools_dir}/grib_set -s forecastTime=60,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=2,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "60 m 2 h"
grib_check_key_equals $temp    "-p $keys__" "1-3 1 3" 
grib_check_key_equals $temp "-y -p $keys__" "1-3 1 3" 
grib_check_key_equals $temp    "-p $keys_s" "1-3 1 3"
grib_check_key_equals $temp "-y -p $keys_s" "1-3 1 3"
grib_check_key_equals $temp    "-p $keys_i" "3 1 3"
grib_check_key_equals $temp "-y -p $keys_i" "3 1 3"
grib_check_key_equals $temp    "-p $keys_d" "3 1 3"
grib_check_key_equals $temp "-y -p $keys_d" "3 1 3"

${tools_dir}/grib_set -s forecastTime=18,indicatorOfUnitOfTimeRange=h,lengthOfTimeRange=6,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "18 h 6 h"
grib_check_key_equals $temp    "-p $keys__" "18-24 18 24" 
grib_check_key_equals $temp "-y -p $keys__" "18-24 18 24"
grib_check_key_equals $temp    "-p $keys_s" "18-24 18 24"
grib_check_key_equals $temp "-y -p $keys_s" "18-24 18 24"
grib_check_key_equals $temp    "-p $keys_i" "24 18 24"
grib_check_key_equals $temp "-y -p $keys_i" "24 18 24"
grib_check_key_equals $temp    "-p $keys_d" "24 18 24"
grib_check_key_equals $temp "-y -p $keys_d" "24 18 24"

${tools_dir}/grib_set -s forecastTime=1080,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=360,indicatorOfUnitForTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "1080 m 360 m"
grib_check_key_equals $temp    "-p $keys__" "18-24 18 24" 
grib_check_key_equals $temp "-y -p $keys__" "18-24 18 24"
grib_check_key_equals $temp    "-p $keys_s" "18-24 18 24"
grib_check_key_equals $temp "-y -p $keys_s" "18-24 18 24"
grib_check_key_equals $temp    "-p $keys_i" "24 18 24"
grib_check_key_equals $temp "-y -p $keys_i" "24 18 24" # TODO(EB): Check if output of stepRange:i makes sense.
grib_check_key_equals $temp    "-p $keys_d" "24 18 24"
grib_check_key_equals $temp "-y -p $keys_d" "24 18 24" # TODO(EB): Check if output of stepRange:d makes sense.

${tools_dir}/grib_set -s forecastTime=1080,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=6,indicatorOfUnitForTimeRange=h $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "1080 m 6 h"
grib_check_key_equals $temp    "-p $keys__" "18-24 18 24" 
grib_check_key_equals $temp "-y -p $keys__" "18-24 18 24"
grib_check_key_equals $temp    "-p $keys_s" "18-24 18 24"
grib_check_key_equals $temp "-y -p $keys_s" "18-24 18 24"
grib_check_key_equals $temp    "-p $keys_i" "24 18 24"
grib_check_key_equals $temp "-y -p $keys_i" "24 18 24"
grib_check_key_equals $temp    "-p $keys_d" "24 18 24"
grib_check_key_equals $temp "-y -p $keys_d" "24 18 24"

rm -f $temp

#~/build/eccodes/bin/grib_ls -m /perm/maro/referenceGRIBfiles4MTG2testing/grib1+2_operational_and_rd/151145_s2_enfo_cf_o2d_zos_2002_prod_ecmf_glob.grib2
#~/build/eccodes/bin/grib_ls -m /perm/maro/referenceGRIBfiles4MTG2testing/grib1+2_operational_and_rd/240023_ce_efas_fc_sfc_dis06_2022_0001_ecmf_lisflood.grib2




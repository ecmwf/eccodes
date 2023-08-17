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
      exit 1
   fi
}

label="grib_ecc-1620"
temp=temp.$label


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
grib_check_key_equals $temp "-y -p $keys_s" "0m"
grib_check_key_equals $temp    "-p $keys_i" "0"
grib_check_key_equals $temp "-y -p $keys_i" "0"
grib_check_key_equals $temp    "-p $keys_d" "0"
grib_check_key_equals $temp "-y -p $keys_d" "0"

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

#grib_check_key_equals $temp "-y -s stepUnits=h -p $keys__" "1.0166666666666666"
#grib_check_key_equals $temp "-y -s stepUnits=h -p $keys_s" "1.0166666666666666"
#grib_check_key_equals $temp "-y -s stepUnits=h -p $keys_i" "1"
#grib_check_key_equals $temp "-y -s stepUnits=h -p $keys_d" "1.0166666666666666"

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
grib_check_key_equals $temp "-y -p $keys_s" "0-2 0m 2"
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

${tools_dir}/grib_set -s forecastTime=45,indicatorOfUnitOfTimeRange=m,lengthOfTimeRange=15,indicatorOfUnitForTimeRange=m $fn $temp
grib_check_key_equals $temp    "-p $low_level_keys" "45 m 15 m"
grib_check_key_equals $temp    "-p $keys__" "45-60 45 60" 
grib_check_key_equals $temp "-y -p $keys__" "45m-60m 45 60" 
grib_check_key_equals $temp    "-p $keys_s" "45-60 45 1"  
grib_check_key_equals $temp "-y -p $keys_s" "45m-60m 45m 1"  
grib_check_key_equals $temp    "-p $keys_i" "60 45 60"  
grib_check_key_equals $temp "-y -p $keys_i" "1 45 60"  
grib_check_key_equals $temp    "-p $keys_d" "60 45 60"  
grib_check_key_equals $temp "-y -p $keys_d" "1 45 60"

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
grib_check_key_equals $temp "-y -p $keys_i" "24 18 24"
grib_check_key_equals $temp    "-p $keys_d" "24 18 24"
grib_check_key_equals $temp "-y -p $keys_d" "24 18 24"

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




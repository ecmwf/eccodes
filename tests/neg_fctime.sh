#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null


temp=neg_fctime.temp.grib
input=${data_dir}/sample.grib2

# check input time and date
check_key_equals $input 'dataDate,dataTime' '20080206 1200'

# Negative step in same day
# --------------------------
${tools_dir}grib_set -s forecastTime=-2 $input $temp
check_key_equals $temp 'validityDate,validityTime' '20080206 1000'

${tools_dir}grib_set -s forecastTime=-11 $input $temp
check_key_equals $temp 'validityDate,validityTime' '20080206 100'

${tools_dir}grib_set -s forecastTime=-12 $input $temp
check_key_equals $temp 'validityDate,validityTime' '20080206 0'


# Negative step previous day
# --------------------------
${tools_dir}grib_set -s forecastTime=-13 $input $temp
check_key_equals $temp 'validityDate,validityTime' '20080205 2300'

${tools_dir}grib_set -s forecastTime=-24 $input $temp
check_key_equals $temp 'validityDate,validityTime' '20080205 1200'

${tools_dir}grib_set -s forecastTime=-33 $input $temp
check_key_equals $temp 'validityDate,validityTime' '20080205 300'

# Negative step further back
# --------------------------
${tools_dir}grib_set -s forecastTime=-38 $input $temp
check_key_equals $temp 'validityDate,validityTime' '20080204 2200'

${tools_dir}grib_set -s forecastTime=-185 $input $temp
check_key_equals $temp 'validityDate,validityTime' '20080129 1900'


# Clean up
rm -f $temp

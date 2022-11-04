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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1150
# ECC-1150: keys 'lowerLimit' & 'upperLimit' cannot be MISSING
# See also ECC-1286
# ---------------------------------------------------------
label="grib_ecc-1150_test"
tempGrib=temp.${label}.grib
tempFilt=temp.${label}.filt

in=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

set_lowerLimit_and_check()
{
    _llim=$1
    _sfac=$2
    _sval=$3

    ${tools_dir}/grib_set -s lowerLimit=$_llim $tempGrib $temp2
    grib_check_key_equals $temp2 \
       scaleFactorOfLowerLimit,scaledValueOfLowerLimit,lowerLimit \
       "$_sfac $_sval $_llim"
}


# Decoding: Lower limit
# ----------------------
${tools_dir}/grib_set -s \
   productDefinitionTemplateNumber=5,scaleFactorOfLowerLimit=missing,scaledValueOfLowerLimit=missing \
   $in $tempGrib

grib_check_key_equals $tempGrib lowerLimit,upperLimit 'MISSING 0'

cat > $tempFilt <<EOF
 print "lower=[lowerLimit], upper=[upperLimit]";
 transient llim_missing = missing(lowerLimit);
 transient ulim_missing = missing(upperLimit);

 assert( llim_missing == 1 );
 assert( ulim_missing == 0 );
EOF
${tools_dir}/grib_filter $tempFilt $tempGrib


# Decoding: upper limit
# -----------------------
${tools_dir}/grib_set -s \
   productDefinitionTemplateNumber=5,scaleFactorOfUpperLimit=missing,scaledValueOfUpperLimit=missing \
   $in $tempGrib

grib_check_key_equals $tempGrib lowerLimit,upperLimit '0 MISSING'

cat > $tempFilt <<EOF
 print "lower=[lowerLimit], upper=[upperLimit]";
 transient llim_missing = missing(lowerLimit);
 transient ulim_missing = missing(upperLimit);

 assert( llim_missing == 0 );
 assert( ulim_missing == 1 );
EOF
${tools_dir}/grib_filter $tempFilt $tempGrib

# Encoding
# ----------
temp2=temp2.${label}.grib
${tools_dir}/grib_set -s lowerLimit=missing,upperLimit=missing  $tempGrib $temp2
grib_check_key_equals $temp2 lowerLimit,upperLimit 'MISSING MISSING'
grib_check_key_equals $temp2 \
  scaleFactorOfLowerLimit,scaledValueOfLowerLimit,scaleFactorOfUpperLimit,scaledValueOfUpperLimit \
  'MISSING MISSING MISSING MISSING'

${tools_dir}/grib_set -s lowerLimit=3.14,upperLimit=missing $tempGrib $temp2
grib_check_key_equals $temp2 lowerLimit,upperLimit '3.14 MISSING'

# Negative values
${tools_dir}/grib_set -s lowerLimit=-6.6,upperLimit=-1.02 $tempGrib $temp2
grib_check_key_equals $temp2 scaleFactorOfLowerLimit,scaledValueOfLowerLimit,lowerLimit "1 -66 -6.6"
grib_check_key_equals $temp2 scaleFactorOfUpperLimit,scaledValueOfUpperLimit,upperLimit "2 -102 -1.02"

#                        input   factor  value
set_lowerLimit_and_check 0           0   0
set_lowerLimit_and_check 1           0   1
set_lowerLimit_and_check 550         0   550
set_lowerLimit_and_check -99         0   -99
set_lowerLimit_and_check 6.77        2   677
set_lowerLimit_and_check 0.001       3   1
set_lowerLimit_and_check -6.6        1   -66
set_lowerLimit_and_check -1.02       2   -102
set_lowerLimit_and_check 3e-05       5   3
set_lowerLimit_and_check -3.9e-05    6   -39
set_lowerLimit_and_check 3.14e-06    8   314
set_lowerLimit_and_check 3.14e-07    9   314
set_lowerLimit_and_check 3e-08       8   3
set_lowerLimit_and_check -3e-08      8   -3
set_lowerLimit_and_check 1e-10       10  1
set_lowerLimit_and_check 3.14e-09    11  314
set_lowerLimit_and_check 1.23e-05    7   123
set_lowerLimit_and_check -3.1456e-09 13  -31456
set_lowerLimit_and_check 9e-126      126 9
set_lowerLimit_and_check 6e-127      127 6


# Clean up
rm -f $tempGrib $tempFilt $temp2

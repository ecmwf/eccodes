#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1150
# ECC-1150: keys 'lowerLimit' & 'upperLimit' cannot be MISSING
# ---------------------------------------------------------
label="grib_ecc-1150-test"
tempGrib=temp.${label}.grib
tempFilt=temp.${label}.filt

in=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

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
${tools_dir}/grib_set -s upperLimit=missing,lowerLimit=missing $tempGrib $temp2
grib_check_key_equals $temp2 lowerLimit,upperLimit 'MISSING MISSING'
grib_check_key_equals $temp2 \
  scaleFactorOfLowerLimit,scaledValueOfLowerLimit,scaleFactorOfUpperLimit,scaledValueOfUpperLimit \
  'MISSING MISSING MISSING MISSING'
rm -f $temp2

# Clean up
rm -f $tempGrib $tempFilt

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

label="grib_set_fail_test"
REDIRECT=/dev/null

outfile=${data_dir}/temp.$label.grib
temp=${data_dir}/temp.$label.out

infile=${data_dir}/regular_gaussian_surface.grib2


# Set without -s
# ----------------------------------------------------
set +e
${tools_dir}/grib_set -p levtype $infile $outfile > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "provide some keys to set" $temp

# Set with empty -s
# ----------------------------------------------------
set +e
${tools_dir}/grib_set -s '' $infile $outfile > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "provide some keys to set" $temp

# Out-of-bounds value
# ----------------------------------------------------
input=${data_dir}/reduced_gaussian_sub_area.grib2
set +e
${tools_dir}/grib_set -s perturbationNumber=1000 $input $outfile 2>$temp
status=$?
set -e
[ $status -ne 0 ]
grep -q "Trying to encode value of 1000 but the maximum allowable value is 255 (number of bits=8)" $temp

# Negative value for an unsigned key
# ----------------------------------------------------
input=${data_dir}/reduced_gaussian_sub_area.grib2
set +e
${tools_dir}/grib_set -s perturbationNumber=-1 $input $outfile 2>$temp
status=$?
set -e
[ $status -ne 0 ]
grep -q "Trying to encode a negative value of -1 for key of type unsigned" $temp

# Bad value for -d
# ----------------
input=${data_dir}/reduced_gaussian_sub_area.grib2
set +e
${tools_dir}/grib_set -d hello $input $outfile 2>$temp
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid number" $temp


# ECC-1605: Out-of-bounds value for signed keys
# ----------------------------------------------------
if [ $ECCODES_ON_WINDOWS -eq 0 ]; then
    input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
    set +e
    ${tools_dir}/grib_set -s forecastTime=2147483648 $input $outfile 2>$temp
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "Trying to encode value of 2147483648 but the allowable range is -2147483647 to 2147483647" $temp

    set +e
    ${tools_dir}/grib_set -s forecastTime=-2147483650 $input $outfile 2>$temp
    status=$?
    set -e
    [ $status -ne 0 ]
fi


# ECC-539: avoid output being the same as input
# -----------------------------------------------
set +e
${tools_dir}/grib_set -s centre=0 $outfile $outfile
status=$?
set -e
[ $status -ne 0 ]

# ECC-1777: Bad date/time
# -------------------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s dataDate=20180229 $input $outfile > $temp 2>&1
grep -q "Date is not valid" $temp

${tools_dir}/grib_set -s dataTime=4261 $input $outfile > $temp 2>&1
grep -q "Time is not valid" $temp

${tools_dir}/grib_set -s dataTime=2501 $input $outfile > $temp 2>&1
grep -q "Time is not valid" $temp

# Note for GRIB1 we DO fail on a bad date! This need to be consistent across editions
input=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
set +e
${tools_dir}/grib_set -s dataDate=20180229 $input $outfile > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "invalid date 20180229" $temp

# 2016 did have 29th Feb
${tools_dir}/grib_set -s dataDate=20160229 $input $outfile


# ECC-1359: string that can be converted to an integer
# ---------------------------------------------------
${tools_dir}/grib_set -s month:s=6 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $outfile
grib_check_key_equals $outfile month 6
# Now try an illegal value: a string that cannot be converted to an integer
set +e
${tools_dir}/grib_set -s month=BAD $ECCODES_SAMPLES_PATH/GRIB2.tmpl $outfile 2> $temp
status=$?
set -e
[ $status -ne 0 ]
grep -q "String cannot be converted to an integer" $temp


# ECC-1363: Does not fail for invalid value for key of type 'double'
# ------------------------------------------------------------------
${tools_dir}/grib_set -s angleOfRotation:s=10.66 $ECCODES_SAMPLES_PATH/rotated_ll_sfc_grib2.tmpl $outfile
grib_check_key_equals $outfile angleOfRotation 10.66
# Now try an illegal value: a string that cannot be converted to an integer
set +e
${tools_dir}/grib_set -s angleOfRotation=BAD $ECCODES_SAMPLES_PATH/rotated_ll_sfc_grib2.tmpl $outfile 2>$temp
status=$?
set -e
[ $status -ne 0 ]
grep -q "String cannot be converted to a double" $temp


# Set ascii key via double or long
# --------------------------------
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=21 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $outfile
${tools_dir}/grib_set -s marsDomain=x $outfile $temp
grib_check_key_equals $temp 'marsDomain' 'x'
set +e
${tools_dir}/grib_set -s marsDomain=9 $outfile $temp
status=$?
set -e
[ $status -ne 0 ]

set +e
${tools_dir}/grib_set -s marsDomain=1.2 $outfile $temp
status=$?
set -e
[ $status -ne 0 ]


# Codetable mismatch
# ------------------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
set +e
${tools_dir}/grib_set -s stepUnits=d $input $outfile > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Unit not found" $temp

set +e
${tools_dir}/grib_set -s centre=ECMF $input $outfile > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "centre: No such code table entry.*Did you mean.*ecmf" $temp

# Overflow/Underflow
# ------------------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s missingValue=9223372036854776666 $input $outfile > $temp 2>&1
grep -q "ECCODES WARNING :  Setting .* causes overflow/underflow" $temp

# ------------------------
# Unreadable message
# ------------------------
echo GRIB > $outfile
set +e
${tools_dir}/grib_set -s edition=2 $outfile /dev/null > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "unreadable message" $temp


# Clean up
rm -f $outfile $temp

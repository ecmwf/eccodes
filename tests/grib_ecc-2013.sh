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

# --------------------------------------------------------------------------
# This is the test for the JIRA issue ECC-2013
# Enforce scale factor and scaled value to be missing for the fixed surfaces
# --------------------------------------------------------------------------

label="grib_ecc-2013_test"
tempGrib=temp.$label.grib
tempSample=temp.$label.tmpl
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Create a sample with weird values for the scaleFactor/scaledValue keys
cat >$tempFilt<<EOF
    set scaleFactorOfFirstFixedSurface  = 87;
    set scaledValueOfFirstFixedSurface  = 88;
    set scaleFactorOfSecondFixedSurface = 89;
    set scaledValueOfSecondFixedSurface = 90;
    write;
EOF
${tools_dir}/grib_filter -o $tempSample $tempFilt $sample_grib2

do_test()
{
    type1=$1
    type2=$2
    res=$3
    echo "Checking $res..."
    cat >$tempFilt<<EOF
        set typeOfFirstFixedSurface         = $type1;
        set scaleFactorOfFirstFixedSurface  = 10;
        set scaledValueOfFirstFixedSurface  = 11;
        set typeOfSecondFixedSurface        = $type2;
        set scaleFactorOfSecondFixedSurface = 12;
        set scaledValueOfSecondFixedSurface = 13;
        write;
EOF
    ${tools_dir}/grib_filter -o $tempGrib $tempFilt $tempSample
    # Decoding a legacy message should still work
    grib_check_key_equals $tempGrib typeOfLevel "$res"

    # Encoding typeOfLevel should set sv/sf keys to missing
    ${tools_dir}/grib_set -s typeOfLevel=$res $tempSample $tempGrib
    grib_check_key_equals $tempGrib scaleFactorOfFirstFixedSurface MISSING
    grib_check_key_equals $tempGrib scaledValueOfFirstFixedSurface MISSING
    grib_check_key_equals $tempGrib scaleFactorOfSecondFixedSurface MISSING
    grib_check_key_equals $tempGrib scaledValueOfSecondFixedSurface MISSING
}

do_test 1 255 surface
do_test 1 8   entireAtmosphere
do_test 1 162 entireLake
do_test 7 255 tropopause
do_test 8 255 nominalTop


# Clean up
rm -f $tempGrib $tempFilt $tempLog $tempOut $tempRef

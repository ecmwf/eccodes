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

# ---------------------------------------------------------------------
# This is the test for JIRA issue ECC-1806
# Change of paramId in conversion from GRIB1 to GRIB2
# ---------------------------------------------------------------------

label="grib_ecc-1806_test"
tempGrib1=temp.$label.1.grib
tempGrib2=temp.$label.2.grib
tempLog=temp.$label.log

sampleGrib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sampleGrib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

test_conversion()
{
    param1=$1 # old paramId in GRIB1
    param2=$2 # new paramId in GRIB2
    short=$3  # new shortName

    ${tools_dir}/grib_set -s paramId=$param1 $sampleGrib1 $tempGrib1
    ${tools_dir}/grib_set -s edition=2       $tempGrib1   $tempGrib2
    grib_check_key_equals $tempGrib2 paramId,shortName "$param2 $short"

    # Check the old paramId has been removed from GRIB2
    set +e
    ${tools_dir}/grib_set -s paramId=$param1 $sampleGrib2 $tempGrib2 >$tempLog 2>&1
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "no match for paramId" $tempLog
}

test_conversion 174098 262000 sithick
test_conversion 151163 262104 t20d
test_conversion 151145 262124 zos
test_conversion 130232 235135 avg_w
test_conversion 172146 235033 avg_ishf
test_conversion 172147 235034 avg_slhtf
test_conversion 55     228004 avg_2t
test_conversion 56     235168 avg_2d
test_conversion 172169 235035 avg_sdswrf
test_conversion 172175 235036 avg_sdlwrf
test_conversion 172176 235037 avg_snswrf
test_conversion 172177 235038 avg_snlwrf
test_conversion 172178 235039 avg_tnswrf
test_conversion 172179 235040 avg_tnlwrf
test_conversion 151175 262118 sc300m

test_conversion 72 260087 sdswrf
test_conversion 73 260097 sdlwrf

# Clean up
rm -f $tempGrib1 $tempGrib2 $tempLog

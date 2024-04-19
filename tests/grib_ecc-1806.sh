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

REDIRECT=/dev/null

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
test_conversion 130232 235135 mw
test_conversion 172146 235033 msshf
test_conversion 172147 235034 mslhf
test_conversion 55     228004 mean2t
test_conversion 56     235168 m2d
test_conversion 172169 235035 msdwswrf
test_conversion 172175 235036 msdwlwrf
test_conversion 172176 235037 msnswrf
test_conversion 172177 235038 msnlwrf
test_conversion 172178 235039 mtnswrf
test_conversion 172179 235040 mtnlwrf


# Clean up
rm -f $tempGrib1 $tempGrib2 $tempLog

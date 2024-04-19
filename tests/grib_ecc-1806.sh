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
tempGribA=temp.$label.1.grib
tempGribB=temp.$label.2.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempRef=temp.$label.ref

sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

test_conversion()
{
    param1=$1 # old paramId in GRIB1
    param2=$2 # new paramId in GRIB2
    short=$3  # new shortName
    ${tools_dir}/grib_set -s paramId=$param1,edition=2 $sample_grib1 $tempGribA
    grib_check_key_equals $tempGribA paramId,shortName "$param2 $short"

    # The old paramId has been removed from GRIB2
    set +e
    ${tools_dir}/grib_set -s paramId=$param1 $sample_grib2 $tempGribA 2>$REDIRECT
    status=$?
    set -e
    [ $status -ne 0 ]

}

test_conversion 174098 262000 sithick
test_conversion 151163 262104 t20d
test_conversion 151145 262124 zos

# Clean up
rm -f $tempGribA $tempGribB $tempFilt $tempLog $tempRef

#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

#set -eax
grib2_sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
this_test=ecCodes-uerra
tempSample=tempSample.${this_test}.grib2
temp1=temp1.${this_test}.grib2
temp2=temp2.${this_test}.grib2

test_stream_and_type()
{
    utype=$1

    if [ "$utype" = "oper" ]; then
        pspd=8
    elif [ "$utype" = "test" ]; then
        pspd=9
    fi

    ${tools_dir}/grib_set -s tablesVersion=15,productionStatusOfProcessedData=$pspd \
                           $grib2_sample $tempSample

    grib_check_key_equals $tempSample 'stream,mars.stream' 'oper oper'
    
    # Check mars model is not set
    model=`${tools_dir}/grib_get -f -p mars.model $tempSample`
    [ "$model" = "not_found" ]

    ${tools_dir}/grib_set -s typeOfProcessedData=0 $tempSample $temp1
    grib_check_key_equals $temp1 'mars.type' 'an'
    ${tools_dir}/grib_set -s typeOfProcessedData=1 $tempSample $temp1
    grib_check_key_equals $temp1 'mars.type' 'fc'

    ${tools_dir}/grib_set -s marsStream=enda $tempSample $temp1    # ENSEMBLE
    grib_check_key_exists $temp1 'perturbationNumber'

    # All combinations
    for mt in an fc; do
        for ms in oper enda; do
            echo "Testing stream=$ms type=$mt ..."
            ${tools_dir}/grib_set -s marsStream=$ms,marsType=$mt $tempSample $temp1
            grib_check_key_equals $temp1 'mars.stream,mars.type' "$ms $mt"

            # Param 228228 (accum) and 167 (instant)
            ${tools_dir}/grib_set -s stepType=accum,paramId=228228 $temp1 $temp2
            grib_check_key_equals $temp2 'stream,type' "$ms $mt"

            ${tools_dir}/grib_set -s paramId=167                   $temp1 $temp2
            grib_check_key_equals $temp2 'stream,type' "$ms $mt"
        done
    done
}

test_stream_and_type 'oper'
test_stream_and_type 'test'

rm -f $temp1 $temp2 $tempSample

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

grib2_sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
label=grib_uerra_test
tempSample=tempSample.${label}.grib2
temp1=temp1.${label}.grib2
temp2=temp2.${label}.grib2

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

# ECC-1428
# ----------
${tools_dir}/grib_set -s marsType=oi $tempSample $temp1
grib_check_key_equals $temp1 'mars.type' 'oi'
set +e
${tools_dir}/grib_set -s marsType=xx $tempSample $temp1
status=$?
set -e
[ $status -ne 0 ]

# ECC-1532
# ---------
# By default crraLocalVersion=1 which does not allow expver to be set
# because it is a constant (prod or test)
set +e
${tools_dir}/grib_set -s \
  productionStatusOfProcessedData=10,grib2LocalSectionPresent=1,marsExpver=coco \
$grib2_sample $temp1
status=$?
set -e
[ $status -ne 0 ]

# ECC-1532
# crraLocalVersion=2 has a coded key for experimentVersionNumber
${tools_dir}/grib_set -s \
  productionStatusOfProcessedData=10,grib2LocalSectionPresent=1,crraLocalVersion=2,marsExpver=coco \
$grib2_sample $temp1
grib_check_key_equals $temp1 'marsExpver,mars.expver' 'coco coco'

${tools_dir}/grib_set -s \
  productionStatusOfProcessedData=11,grib2LocalSectionPresent=1,crraLocalVersion=2,experimentVersionNumber=0078 \
$grib2_sample $temp1
grib_check_key_equals $temp1 'marsExpver,mars.expver' '0078 0078'


# Clean up
rm -f $temp1 $temp2 $tempSample

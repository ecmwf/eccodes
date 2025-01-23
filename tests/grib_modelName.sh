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

# See ECC-1846

label="grib_modelName_test"
tempGrib=temp.$label.grib
temp2Grib=temp2.$label.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s generatingProcessIdentifier=1,backgroundProcess=1 $sample $tempGrib
grib_check_key_equals $tempGrib modelName,modelVersion "aifs-single-mse v1"

${tools_dir}/grib_set -s generatingProcessIdentifier=154 $sample $tempGrib
grib_check_key_equals $tempGrib modelName,modelVersion "IFS cy48r1"

${tools_dir}/grib_set -s generatingProcessIdentifier=100 $sample $tempGrib
grib_check_key_equals $tempGrib modelName,modelVersion "IFS unknown"

# Check that this only works for centre ecmf
${tools_dir}/grib_set -s generatingProcessIdentifier=1,backgroundProcess=1,centre=84 $sample $tempGrib
[ $( ${tools_dir}/grib_get -f -p modelName $tempGrib ) = "not_found" ]
[ $( ${tools_dir}/grib_get -f -p modelVersion $tempGrib ) = "not_found" ]

# Check that it works for pseudocentres (e.g. DestinE)
${tools_dir}/grib_set -s generatingProcessIdentifier=1,backgroundProcess=3,productionStatusOfProcessedData=13 $sample $tempGrib
grib_check_key_equals $tempGrib modelName,modelVersion "ALARO cy46h1"
${tools_dir}/grib_set -s productionStatusOfProcessedData=0 $tempGrib $temp2Grib
[ $( ${tools_dir}/grib_get -f -p modelName $temp2Grib ) = "aifs-ens-diff" ]
[ $( ${tools_dir}/grib_get -f -p modelVersion $temp2Grib ) = "v1" ]


# ECC-2010
${tools_dir}/grib_set -s marsClass=ai,typeOfProcessedData=missing,backgroundProcess=1,generatingProcessIdentifier=1 \
    $ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl $temp2Grib
grib_check_key_equals $temp2Grib mars.model 'aifs-single-mse'

${tools_dir}/grib_set -s marsClass=ai,typeOfProcessedData=missing,backgroundProcess=2,generatingProcessIdentifier=1 \
    $ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl $temp2Grib
grib_check_key_equals $temp2Grib mars.model 'aifs-ens-crps'


# Keys are read-only (may change this later)
set +e
${tools_dir}/grib_set -s modelName=AIFS $sample $tempGrib 2>$tempLog
status=$?
set -e
[ $status -ne 0 ]
grep -q "Value is read only" $tempLog

set +e
${tools_dir}/grib_set -s modelVersion=cy48r1 $sample $tempGrib 2>$tempLog
status=$?
set -e
[ $status -ne 0 ]
grep -q "Value is read only" $tempLog


# Clean up
rm -f $tempGrib $tempFilt $tempLog $tempOut $tempRef
rm -f $temp2Grib

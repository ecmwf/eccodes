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

label="grib_ecc-1846_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s generatingProcessIdentifier=1,backgroundProcess=1 $sample $tempGrib
grib_check_key_equals $tempGrib modelName,modelVersion "AIFS v1"

${tools_dir}/grib_set -s generatingProcessIdentifier=154 $sample $tempGrib
grib_check_key_equals $tempGrib modelName,modelVersion "IFS cy48r1"

${tools_dir}/grib_set -s generatingProcessIdentifier=100 $sample $tempGrib
grib_check_key_equals $tempGrib modelName,modelVersion "IFS unknown"

# Clean up
rm -f $tempGrib $tempFilt $tempLog $tempOut $tempRef

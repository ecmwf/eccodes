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

label="grib_accessors_test"
tempText=temp.$label.txt

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

# Accessors like uint16, uint32 etc
# ----------------------------------
tempDir=${label}.temp.dir
rm -rf $tempDir
mkdir -p $tempDir/definitions/grib2
bootfile=$tempDir/definitions/grib2/boot.def
cat $def_dir/grib2/boot.def > $bootfile
echo "uint16 key_uint16: transient;"    >> $bootfile
echo "uint32 key_uint32: transient;"    >> $bootfile
echo "uint32_little_endian key_uint32_le: transient;"  >> $bootfile

echo "uint64 key_uint64: transient;"    >> $bootfile
echo "uint64_little_endian key_uint64_le: transient;"  >> $bootfile

echo "key_tos1 = to_string(kindOfProduct,0,1);"  >> $bootfile
echo "key_tos2 = to_string(dataDate,0,3);"       >> $bootfile
echo >> $bootfile

curr_defs=$ECCODES_DEFINITION_PATH
export ECCODES_DEFINITION_PATH=$PWD/$tempDir/definitions:$curr_defs

input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
set +e
# Not yet fully implemented
${tools_dir}/grib_get -p key_uint16    $input > $tempText 2>&1
${tools_dir}/grib_get -p key_uint32    $input >> $tempText 2>&1
${tools_dir}/grib_get -p key_uint32_le $input >> $tempText 2>&1
set -e

# TODO
# grib_check_key_equals $input 'key_uint64,key_uint64_le' '0 0'

# kindOfProduct = GRIB, dataDate = 20070323
grib_check_key_equals $input 'key_tos1,key_tos2'     'G 200'
grib_check_key_equals $input 'key_tos2:d,key_tos2:i' '200 200'

rm -rf $tempDir


# Clean up
rm -f $tempText

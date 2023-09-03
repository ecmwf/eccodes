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

label="grib_get_fail_test"
tempText=temp.$label.txt

# Check input file has been downloaded
[ -f ${data_dir}/regular_latlon_surface.grib1 ]

# Expect failure as the key does not exist
set +e
${tools_dir}/grib_get -p boomerang ${data_dir}/regular_latlon_surface.grib1
status=$?
set -e
[ $status -ne 0 ]

# ECC-1551: Print which key does not exist
# -----------------------------------------
set +e
${tools_dir}/grib_get -p Ni,Nh,Nj $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Nh (Key/value not found)" $tempText

# Types like uint16, uint32 etc
# -------------------------------
if [ $ECCODES_ON_WINDOWS -eq 0 ]; then
    tempDir=${label}.temp.dir
    rm -rf $tempDir
    mkdir -p $tempDir/definitions/grib2
    bootfile=$tempDir/definitions/grib2/boot.def
    cat $def_dir/grib2/boot.def > $bootfile
    echo "uint16 key_uint16: transient;"    >> $bootfile
    echo "uint32 key_uint32: transient;"    >> $bootfile
    echo "uint64 key_uint64: transient;"    >> $bootfile
    echo "uint32_little_endian key_uint32_le: transient;"    >> $bootfile
    echo "uint64_little_endian key_uint64_le: transient;"    >> $bootfile
    curr_defs=$ECCODES_DEFINITION_PATH
    export ECCODES_DEFINITION_PATH=$PWD/$tempDir/definitions:$curr_defs

    input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
    set +e
    # Not yet fully implemented
    ${tools_dir}/grib_get -p key_uint16    $input > $tempText 2>&1
    ${tools_dir}/grib_get -p key_uint32    $input >> $tempText 2>&1
    ${tools_dir}/grib_get -p key_uint32_le $input >> $tempText 2>&1
    set -e
    cat $tempText

    ${tools_dir}/grib_get -p key_uint64    $input
    ${tools_dir}/grib_get -p key_uint64_le $input

    rm -rf $tempDir
fi

# Clean up
rm -f $tempText

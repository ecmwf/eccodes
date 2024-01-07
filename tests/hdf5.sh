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

label="hdf5_test"

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

tempOut=temp.${label}.out
tempTxt=temp.${label}.txt
tempRef=temp.${label}.ref

input=$ECCODES_SAMPLES_PATH/hdf5.tmpl

${tools_dir}/grib_dump -TA -O $input > $tempTxt
grep -q "versionNumberOfSuperblock = 2" $tempTxt
grep -q "endOfFileAddress = 342"        $tempTxt

id=`${tools_dir}/grib_get -TA -p identifier $input`
[ "$id" = "HDF5" ]

# Clean up
rm -f $tempOut $tempRef $tempTxt

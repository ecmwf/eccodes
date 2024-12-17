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

label="codes_assert_test"

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

tempOut=temp.$label.txt
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

tempDir=${label}.temp.dir
rm -rf $tempDir
mkdir -p $tempDir/definitions/grib2
bootfile=$tempDir/definitions/grib2/boot.def
cat $def_dir/grib2/boot.def > $bootfile
echo 'assert( year == 1990 );' >> $bootfile
echo >> $bootfile

export ECCODES_DEFINITION_PATH=$PWD/$tempDir/definitions:$ECCODES_DEFINITION_PATH
# This will activate the print statement above
${tools_dir}/grib_set -s year=1990 $sample_grib2 /dev/null

set +e
${tools_dir}/grib_set -s year=1991 $sample_grib2 /dev/null > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "year.*Assertion failure" $tempOut

# Clean up
rm -rf $tempDir
rm -f $tempOut

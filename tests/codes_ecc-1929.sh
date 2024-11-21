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

label="codes_ecc-1929_test"

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
echo 'print "DEBUG: [gridType=] [typeOfLevel=]";' >> $bootfile
echo >> $bootfile

export ECCODES_DEFINITION_PATH=$PWD/$tempDir/definitions:$ECCODES_DEFINITION_PATH
# This will activate the print statement above
${tools_dir}/grib_get -p edition $sample_grib2 > $tempOut
grep -q "DEBUG: gridType=regular_ll typeOfLevel=surface" $tempOut

# Clean up
rm -rf $tempDir
rm -f $tempOut

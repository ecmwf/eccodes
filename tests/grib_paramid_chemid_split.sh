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

label="grib__test"  # Change prod to bufr or grib etc
tempGrib1=temp1.$label.grib
tempGrib2=temp2.$label.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# we expect paramId 210121
${tools_dir}/grib_set -s productDefinitionTemplateNumber=40,discipline=0,parameterCategory=20,parameterNumber=2,constituentType=5 $sample_grib2  $tempGrib1
# we expect paramId 402000 and chemId 17
${tools_dir}/grib_set -s tablesVersion=32 $tempGrib1 $tempGrib2

grib_check_key_equals $tempGrib1 paramId "210121"
grib_check_key_equals $tempGrib2 paramId,chemId "402000 17"

# Clean up
rm -f $tempGrib1 $tempGrib2 

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

# ------------------------------------------------------------
# This is the test for the JIRA issue ECC-1942
# Reorganise template.4.horizontal.def to avoid unaliasing mars.levelist
# ------------------------------------------------------------

REDIRECT=/dev/null

label="grib_ecc-1942_test"
tempGrib=temp.$label.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s \
    typeOfFirstFixedSurface=102,typeOfSecondFixedSurface=255,gridDefinitionTemplateNumber=101,numberOfGridUsed=1,level=44 \
    $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib levtype o2d
grib_check_key_equals $tempGrib mars.levtype o2d

result=$( ${tools_dir}/grib_get -fp mars.levelist $tempGrib )
[ "$result" = "not_found" ]

result=$( ${tools_dir}/grib_get -fp levelist $tempGrib )
[ "$result" = "not_found" ]

# Clean up
rm -f $tempGrib

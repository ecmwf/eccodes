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

label="grib2_paramid_chemid_split_test"
tempGribA=temp1.$label.grib
tempGribB=temp2.$label.grib

sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Legacy using older tablesVersion
# We expect paramId 210121
${tools_dir}/grib_set -s productDefinitionTemplateNumber=40,discipline=0,parameterCategory=20,parameterNumber=2,constituentType=5 \
            $sample $tempGribA
grib_check_key_equals $tempGribA tablesVersion 4
grib_check_key_equals $tempGribA paramId,is_chemical "210121 1"
# The new keys should not be present
result=$( ${tools_dir}/grib_get -f -p chemName,chemId $tempGribA )
[ "$result" = "not_found not_found" ]


# Switch to the newer tablesVersion so now the chemId/paramId split is activated
# Now expect paramId 402000 and chemId 17
${tools_dir}/grib_set -s tablesVersion=32 $tempGribA $tempGribB

grib_check_key_equals $tempGribB paramId,chemId "402000 17"
grib_check_key_equals $tempGribB chemName "Nitrogen dioxide"
grib_check_key_equals $tempGribB name "Mass mixing ratio"

# The is_chemical key should not be present
result=$( ${tools_dir}/grib_get -f -p is_chemical $tempGribB )
[ "$result" = "not_found" ]


# Test non-chemical e.g. temperature. Now chem keys are present but unknown
${tools_dir}/grib_set -s tablesVersion=32,discipline=0,parameterCategory=0,parameterNumber=0 $sample  $tempGribA
grib_check_key_equals $tempGribA "chemId,chemName,chemShortName" "0 unknown unknown"


# Clean up
rm -f $tempGribA $tempGribB 

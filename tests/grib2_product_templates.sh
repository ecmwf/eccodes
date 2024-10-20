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

label="grib2_product_templates_test"
tempText=temp.$label.txt
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib

if [ ! -d "$ECCODES_DEFINITION_PATH" ]; then
    echo "Test $0 disabled. No definitions directory"
    exit 0
fi

sample_g2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
def_file="$ECCODES_DEFINITION_PATH/grib2/productDefinitionTemplateConcept.def"

# Ensure all productDefinitionTemplateNumber values are unique
# ------------------------------------------------------------
rm -f $tempText

# uniq -d outputs a single copy of each line that is repeated in the input
awk -F= '$0 ~ /product/ {print $3}' $def_file | tr -d ';} ' | sort -n | uniq -d > $tempText

if [ -s "$tempText" ]; then
    # File exists and has a size greater than zero
    echo "ERROR: Duplicate PDT number found in $def_file" >&2
    cat $tempText
    exit 1
else
    echo "No duplicates in $def_file"
fi

# Ensure all strings are unique
# ------------------------------
rm -f $tempText
grep "^'" $def_file | awk -F= '{print $1}' | tr -d "' " | sort | uniq -d > $tempText
cat $tempText
if [ -s "$tempText" ]; then
    # File exists and has a size greater than zero
    echo "ERROR: Duplicate string found in $def_file" >&2
    cat $tempText
    exit 1
else
    echo "No duplicates in $def_file"
fi

# Automatic PDT selection
# from instantaneous to statistically processed (accum,max,etc)
# -------------------------------------------------------------
grib_check_key_equals $sample_g2 productDefinitionTemplateNumber,stepType '0 instant'
$tools_dir/grib_set -s shortName=tp $sample_g2 $tempGribA
grib_check_key_equals $tempGribA productDefinitionTemplateNumber 8
grib_check_key_equals $tempGribA typeOfStatisticalProcessing,stepType '1 accum'
grib_check_key_equals $tempGribA shortName,name 'tp Total precipitation'

$tools_dir/grib_set -s productDefinitionTemplateNumber=1,shortName=tp,perturbationNumber=32 $sample_g2 $tempGribA
grib_check_key_equals $tempGribA productDefinitionTemplateNumber 11
grib_check_key_equals $tempGribA number 32
grib_check_key_equals $tempGribA typeOfStatisticalProcessing,stepType '1 accum'
grib_check_key_equals $tempGribA shortName,name 'tp Total precipitation'

$tools_dir/grib_set -s paramId=237382 $sample_g2 $tempGribA
$tools_dir/grib_ls -jn parameter $tempGribA
grib_check_key_equals $tempGribA productDefinitionTemplateNumber 8
grib_check_key_equals $tempGribA typeOfStatisticalProcessing,stepType '2 max'
grib_check_key_equals $tempGribA shortName,name 'max_visp Time-maximum visibility through precipitation'

# Test a failure. paramId=140114 contains wave keys
set +e
$tools_dir/grib_set -s paramId=140114 $sample_g2 $tempGribA 2>$tempText
status=$?
set -e
[ $status -ne 0 ]
# cat $tempText
# check for "typeOfWavePeriodInterval (type=long) failed: Key/value not found" in error log


# Clean up
rm -f $tempText $tempGribA $tempGribB

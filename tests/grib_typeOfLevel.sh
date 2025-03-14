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

label="grib_typeOfLevel_test"
tempText=temp.$label.txt
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib

if [ ! -d "$ECCODES_DEFINITION_PATH" ]; then
    echo "Test $0 disabled. No definitions directory"
    exit 0
fi

sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample_g2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Make sure all typeOfLevel values are unique
# --------------------------------------------
def_file="$ECCODES_DEFINITION_PATH/grib2/typeOfLevelConcept.def"

# Check each entry in the typeOfLevelConcept.def and ensure the equivalent
# mars.levtype exists (not unknown)
types_of_level=$(grep "^'" $def_file | awk -F= '{print $1}' | tr -d "' " | sort -u)
for tol in $types_of_level; do
    # echo "Doing $tol.... levtype="
    ${tools_dir}/grib_set -s typeOfLevel=$tol $sample_g2 $tempGribA
    ${tools_dir}/grib_get -p levtype $tempGribA
done

rm -f $tempText
# uniq -d outputs a single copy of each line that is repeated in the input
# grep "^'" $def_file | awk -F= '{print $1}' | tr -d "' " | sort | uniq -d > $tempText
# if [ -s "$tempText" ]; then
#     # File exists and has a size greater than zero
#     echo "ERROR: Duplicates found in $def_file" >&2
#     cat $tempText
#     exit 1
# else
#     echo "No duplicates in $def_file"
# fi

# ECC-1847: Setting invalid value for 'typeOfLevel' does not fail
# ----------------------------------------------------------------
for sample in $sample_g1 $sample_g2; do
    set +e
    ${tools_dir}/grib_set -s typeOfLevel=rubbish $sample $tempGribA 2>$tempText
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "Concept no match" $tempText
done

# Change of PDT with typeOfLevel=unknown
${tools_dir}/grib_set -s productDefinitionTemplateNumber=0,typeOfFirstFixedSurface=0 $sample_g2 $tempGribA
grib_check_key_equals $tempGribA typeOfLevel 'unknown'
${tools_dir}/grib_set -s productDefinitionTemplateNumber=1 $tempGribA $tempGribB
grib_check_key_equals $tempGribB typeOfLevel,productDefinitionTemplateNumber 'unknown 1'


# ECC-1847: Fix case when input has typeOfLevel=unknown
# In definitions/grib2/typeOfLevelConcept.def, make sure we map 'unknown' correctly (transient dummyc)
${tools_dir}/grib_set -s centre=ecmf,typeOfFirstFixedSurface=254 $sample_g2 $tempGribA
grib_check_key_equals $tempGribA typeOfLevel abstractLevel

# ECC-2013
# Create a badly encoded GRIB with invalid sf/sv values
${tools_dir}/grib_filter -o $tempGribA - $sample_g2 <<EOF
    set typeOfFirstFixedSurface=1;
    set scaleFactorOfFirstFixedSurface=5;
    set scaledValueOfFirstFixedSurface=12;
    set typeOfSecondFixedSurface=255;
    set scaleFactorOfSecondFixedSurface=6;
    set scaledValueOfSecondFixedSurface=11;
    write;
EOF
# It should still match surface
grib_check_key_equals $tempGribA typeOfLevel surface
# Changing the PDT should not tamper with the sf/sv keys
grib_check_key_equals $tempGribA productDefinitionTemplateNumber 0
${tools_dir}/grib_set -s productDefinitionTemplateNumber=8 $tempGribA $tempGribB
${tools_dir}/grib_compare -b totalLength,section4Length,productDefinitionTemplateNumber $tempGribA $tempGribB
# Setting typeOfLevel=surface should set the correct values for sf/sv
${tools_dir}/grib_set -s typeOfLevel=surface $tempGribA $tempGribB
set +e
${tools_dir}/grib_compare $tempGribA $tempGribB
status=$?
set -e
[ $status -ne 0 ]
# New GRIB is still surface but has sf/sv set to missing
grib_check_key_equals $tempGribB typeOfLevel surface
${tools_dir}/grib_compare -b \
   scaleFactorOfFirstFixedSurface,scaledValueOfFirstFixedSurface,scaleFactorOfSecondFixedSurface,scaledValueOfSecondFixedSurface \
   $tempGribA $tempGribB
grib_check_key_equals $tempGribB scaleFactorOfSecondFixedSurface,scaledValueOfFirstFixedSurface 'MISSING MISSING'
grib_check_key_equals $tempGribB scaleFactorOfSecondFixedSurface,scaledValueOfFirstFixedSurface 'MISSING MISSING'

# ECC-2013
# Check copying works OK for typeOfFirstFixedSurface and typeOfLevel
${tools_dir}/grib_set -s paramId=167,productDefinitionTemplateNumber=8 $sample_g2 $tempGribA
grib_check_key_equals $tempGribA typeOfFirstFixedSurface:i,paramId,shortName '103 167 2t'

${tools_dir}/grib_set -s productDefinitionTemplateNumber=8,paramId=167 $sample_g2 $tempGribA
grib_check_key_equals $tempGribA typeOfFirstFixedSurface:i,paramId,shortName '103 167 2t'


# Clean up
rm -f $tempText $tempGribA $tempGribB

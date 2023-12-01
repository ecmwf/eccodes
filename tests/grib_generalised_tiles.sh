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

label="grib_generalised_tiles_test"
temp_grib_a=temp.$label.a.grib
temp_grib_b=temp2.$label.b.grib
temp_dump=temp.$label.dump
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

latest=`${tools_dir}/grib_get -p tablesVersionLatest $sample_grib2`

${tools_dir}/grib_set -s productDefinitionTemplateNumber=113,tablesVersion=$latest ${sample_grib2} ${temp_grib_a}

# Check tile related keys are present

grib_check_key_exists ${temp_grib_a} tileClassification,typeOfTile,numberOfUsedSpatialTiles,numberOfUsedTileAttributeCombinationsForTypeOfTile

grib_check_key_exists ${temp_grib_a} numberOfUsedTileAttributesForTileAttributeCombination,attributeOfTile,totalNumberOfTileAttributeCombinations

grib_check_key_exists ${temp_grib_a} tileIndex,uuidOfDataGroup

# Check list is working correctly and that associated concept is working correctly

echo 'set numberOfUsedTileAttributesForTileAttributeCombination=2; set attributeOfTile={2, 4}; write;' | ${tools_dir}/grib_filter -o ${temp_grib_b} - ${temp_grib_a}
output=$(echo 'print "[attributeOfTile]";' | ${tools_dir}/grib_filter - ${temp_grib_b})
[ "$output" = "2 4" ]
[ "$(${tools_dir}/grib_get -p tileAttribute ${temp_grib_b})" = "SNOW_ICE" ]

# Check template is being picked up correctly

${tools_dir}/grib_dump -O -p section_4 $temp_grib_a > $temp_dump
grep -q "Generalised tiles at a horizontal level or horizontal layer at a point in time" $temp_dump

# Check StatisticalProcessing template also works

${tools_dir}/grib_set -s productDefinitionTemplateNumber=114,tablesVersion=$latest ${sample_grib2} ${temp_grib_a}

grib_check_key_exists ${temp_grib_a} typeOfTile,typeOfStatisticalProcessing

# Check Ensemble template also works

${tools_dir}/grib_set -s productDefinitionTemplateNumber=115,tablesVersion=$latest ${sample_grib2} ${temp_grib_a}

grib_check_key_exists ${temp_grib_a} typeOfTile,perturbationNumber

# Check Ensemble StatisticalProcessing template also works

${tools_dir}/grib_set -s productDefinitionTemplateNumber=116,tablesVersion=$latest ${sample_grib2} ${temp_grib_a}

grib_check_key_exists ${temp_grib_a} typeOfTile,perturbationNumber,typeOfStatisticalProcessing

# Clean up
rm -f $temp_grib_a $temp_grib_b $temp_dump

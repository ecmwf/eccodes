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
set -u
REDIRECT=/dev/null
label="grib_generalised_tiles_test"
temp_grib=temp.$label.grib
temp2_grib=temp2.$label.grib
temp_dump=temp.$label.dump
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s productDefinitionTemplateNumber=113,tablesVersion=30 ${sample_grib2} ${temp_grib}

# Check tile related keys are present

grib_check_key_exists ${temp_grib} tileClassification,typeOfTile,numberOfUsedSpatialTiles,numberOfUsedTileAttributeCombinationsForTypeOfTile

grib_check_key_exists ${temp_grib} numberOfUsedTileAttributesForTileAttributeCombination,attributeOfTile,totalNumberOfTileAttributeCombinations

grib_check_key_exists ${temp_grib} tileIndex,uuidOfDataGroup

# Check list is working correctly and that associated concept is working correctly

echo 'set numberOfUsedTileAttributesForTileAttributeCombination=2; set attributeOfTile={2, 4}; write;' | grib_filter - ${temp_grib} -o ${temp2_grib}
output=$(echo 'print "[attributeOfTile]";' | grib_filter - ${temp2_grib})
[ "$output" == "2 4" ]
[ "$(grib_get -p tileAttribute ${temp2_grib})" == "SNOW_ICE" ]

# Check template is being picked up correctly

${tools_dir}/grib_dump -O -p section_4 $temp_grib > $temp_dump
grep -q "Generalised spatio-temporal changing tiles at a horizontal level or horizontal layer at a point in time" $temp_dump

# Check StatisticalProcessing template also works

${tools_dir}/grib_set -s productDefinitionTemplateNumber=114,tablesVersion=30 ${sample_grib2} ${temp_grib}

grib_check_key_exists ${temp_grib} typeOfTile,typeOfStatisticalProcessing

#...
#infile=${data_dir}/SOME_FILE
#${tools_dir}/grib_get
#${tools_dir}/grib_set
#grib_check_key_equals $temp k1,k2 "v1 v2"
#${tools_dir}/bufr_get
#${tools_dir}/bufr_set
#...

rm -f $temp_grib $temp2_grib $temp_dump

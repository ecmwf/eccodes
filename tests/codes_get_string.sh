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

label="codes_get_string_test"
tempGrib=temp.$label.grib
tempText=temp.$label.txt

input=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
$EXEC ${test_dir}/codes_get_string GRIB $input dataDate

${tools_dir}/grib_set -s marsType=s3,marsStream=mpic $input $tempGrib
$EXEC ${test_dir}/codes_get_string GRIB $tempGrib dayOfTheYearDate # 2> $tempText


input=$data_dir/reduced_latlon_surface.grib2
keys="hundred identifier projString bitmap class year gridDefinitionDescription
      time validityTime packingType md5Headers parameterUnits"
for k in $keys; do
    $EXEC ${test_dir}/codes_get_string GRIB $input $k 2> $tempText
    grep -q "Buffer too small" $tempText
done

input=$ECCODES_SAMPLES_PATH/reduced_gg_ml_grib2.tmpl
$EXEC ${test_dir}/codes_get_string GRIB "$input" gridName 2> $tempText
grep -q "Buffer too small" $tempText


# shortName = swh
input=$data_dir/reduced_latlon_surface.grib1
$EXEC ${test_dir}/codes_get_string GRIB "$input" shortName 2> $tempText
grep -q "Buffer too small" $tempText

input=$data_dir/gts/EGRR20150317121020_00493212.DAT
$EXEC ${test_dir}/codes_get_string GTS "$input" theMessage 2> $tempText
grep -q "Buffer too small" $tempText

# Clean up
rm -f $tempText

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
tempText=temp.$label.txt

input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

input=$data_dir/reduced_latlon_surface.grib2
keys="identifier projString bitmap class year gridDefinitionDescription
      time validityTime packingType md5Headers parameterUnits"
for k in $keys; do
    $EXEC ${test_dir}/codes_get_string $input $k 2> $tempText
    grep -q "Buffer too small" $tempText
done

input=$ECCODES_SAMPLES_PATH/reduced_gg_ml_grib2.tmpl
$EXEC ${test_dir}/codes_get_string "$input" gridName 2> $tempText
grep -q "Buffer too small" $tempText


# shortName = swh
input=$data_dir/reduced_latlon_surface.grib1
$EXEC ${test_dir}/codes_get_string "$input" shortName 2> $tempText
grep -q "Buffer too small" $tempText


rm -f $tempText

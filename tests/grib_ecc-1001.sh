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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1001
# grib_ls -j -l command (JSON for nearest)
# ---------------------------------------------------------
label="grib_ecc-1001_test"
tempOut1=temp.${label}.1.out
tempOut2=temp.${label}.2.out

# Decide if we have the JSON verifier commandline utility
JSON_VERIF="json_xs"
JSON_CHECK=""
if command -v $JSON_VERIF >/dev/null 2>&1; then
  JSON_CHECK=$JSON_VERIF
fi
# ECC-1119: Check the json_xs command actually works!
set +e
echo '[]' | json_xs > /dev/null 2>&1
if [ $? -ne 0 ]; then JSON_CHECK=""; fi
set -e
echo "Using $JSON_CHECK ..."

inputs="
  $ECCODES_SAMPLES_PATH/GRIB1.tmpl
  $ECCODES_SAMPLES_PATH/GRIB2.tmpl
  ${data_dir}/high_level_api.grib2
"

for f in $inputs; do
    rm -f $tempOut1 $tempOut2
    ${tools_dir}/grib_ls -m -j -l 51.0,1.0,1 $f > $tempOut1
    ${tools_dir}/grib_ls -m -j -l 51.0,1.0,4 $f > $tempOut2
    if test "x$JSON_CHECK" != "x"; then
        $JSON_VERIF -t none < $tempOut1
        $JSON_VERIF -t none < $tempOut2
    fi
done


# Clean up
rm -f $tempOut1 $tempOut2

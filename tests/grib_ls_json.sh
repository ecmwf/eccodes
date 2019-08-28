#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

tempLog=temp.ls.json.log
rm -f $tempLog

cd ${data_dir}

# Test a MISSING key
# --------------------
input=sample.grib2
${tools_dir}/grib_ls -j -p scaledValueOfEarthMajorAxis $input > $tempLog
grep -q "scaledValueOfEarthMajorAxis.*MISSING" $tempLog


# Test decoding a given key in different ways
# ---------------------------------------------
input=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
${tools_dir}/grib_ls -j -p levelType,levelType:i  $input > $tempLog
grep -q "levelType.*pl" $tempLog
grep -q "levelType.*100" $tempLog


# Check JSON validity
# --------------------
# Decide if we have the JSON verifier commandline utility
JSON_VERIF="json_xs"
JSON_CHECK=""
if command -v $JSON_VERIF >/dev/null 2>&1; then
  JSON_CHECK=$JSON_VERIF
fi

grib_files=`cat ${data_dir}/grib_data_files.txt`
for file in ${grib_files}
do
  input=${data_dir}/$file
  ${tools_dir}/grib_ls -j $input > $tempLog
  if test "x$JSON_CHECK" != "x"; then
    json_xs -t none < $tempLog
  fi
done


# Clean up
rm -f $tempLog

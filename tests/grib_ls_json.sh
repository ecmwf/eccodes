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

tempLog=temp.grib_ls_json.log
tempOut=temp.grib_ls_json.txt
tempRef=temp.grib_ls_json.ref
rm -f $tempLog $tempOut $tempRef

# Decide if we have the JSON verifier commandline utility
JSON_VERIF="json_xs"
JSON_CHECK=""
if command -v $JSON_VERIF >/dev/null 2>&1; then
  JSON_CHECK=$JSON_VERIF
fi

cd ${data_dir}

# Check there is no "not_found" for a mixed GRIB file
# ----------------------------------------------------
input=mixed.grib
# Normally dataType will not exist for the kwbc messages so we should
# get several 'not_found' strings
${tools_dir}/grib_ls $input > $tempLog
grep -q 'not_found' $tempLog

# With the JSON option grib_ls will not use the first message's keys
# so there should not be any instances of 'not_found'
${tools_dir}/grib_ls -j $input > $tempLog
if grep -q 'not_found' $tempLog; then
  echo "grib_ls: JSON output should not have contained 'not_found'"
  exit 1
fi


# Test ordering
# ----------------------------------------------------
input=high_level_api.grib2
${tools_dir}/grib_ls -j -B'step:i desc' $input > $tempLog

if test "x$JSON_CHECK" != "x"; then
  json_xs -t none < $tempLog
fi

grep stepRange $tempLog > $tempOut
cat > $tempRef <<EOF
    "stepRange": 24,
    "stepRange": 18,
    "stepRange": 12,
    "stepRange": 6,
    "stepRange": 0,
EOF
diff $tempRef $tempOut

# Test a MISSING key
# ----------------------------------------------------
input=sample.grib2
${tools_dir}/grib_ls -j -p scaledValueOfEarthMajorAxis $input > $tempLog
grep -q '"scaledValueOfEarthMajorAxis": "MISSING"' $tempLog


# Test decoding a given key as string and integer
# ----------------------------------------------------
input=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
${tools_dir}/grib_ls -j -p levelType,levelType:i  $input > $tempLog
grep -q '"levelType": "pl"' $tempLog
grep -q '"levelType": 100'  $tempLog


# Test decoding floating point key with format
# ----------------------------------------------------
input=$ECCODES_SAMPLES_PATH/reduced_gg_pl_128_grib2.tmpl
${tools_dir}/grib_ls -j -p latitudeOfLastGridPointInDegrees $input > $tempLog
grep -q '"latitudeOfLastGridPointInDegrees": -89.4628' $tempLog

${tools_dir}/grib_ls -F%.3f -j -p latitudeOfLastGridPointInDegrees $input > $tempLog
grep -q '"latitudeOfLastGridPointInDegrees": -89.463' $tempLog


# Check output from all our downloaded GRIBs
# ----------------------------------------------------
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
rm -f $tempLog $tempOut $tempRef

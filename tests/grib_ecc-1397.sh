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
# This is the test for the JIRA issue ECC-1397
# grib_ls -j -l command (JSON for nearest)
# ---------------------------------------------------------
label="grib_ecc-1397_test"
tempOut1=temp.${label}.1.out
tempOut2=temp.${label}.2.out

# Decide if we have the JSON verifier commandline utility
JSON_VERIF="json_xs"
JSON_CHECK=""
if command -v $JSON_VERIF >/dev/null 2>&1; then
  JSON_CHECK=$JSON_VERIF
fi
# Check the json_xs command actually works!
set +e
echo '[]' | json_xs > /dev/null 2>&1
if [ $? -ne 0 ]; then JSON_CHECK=""; fi
set -e
echo "Using $JSON_CHECK ..."


# Check number of lines printed
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_ls -j -l 51.0,1.0,1 $input > $tempOut1
numlines1=`wc -l $tempOut1 | awk '{print $1}'`
[ $numlines1 -eq 10 ]

${tools_dir}/grib_ls -j -l 51.0,1.0,4 $input > $tempOut2
numlines2=`wc -l $tempOut2 | awk '{print $1}'`
[ $numlines2 -eq 13 ]

# When mode=1, do not print "selected"
set +e
grep -q "selected" $tempOut1
status=$?
set -e
[ $status -ne 0 ]

# It should be there when mode=4
grep -q "selected" $tempOut2

# Verify validity of JSON
if test "x$JSON_CHECK" != "x"; then
    $JSON_VERIF < $tempOut1
    $JSON_VERIF < $tempOut2
fi


# Clean up
rm -f $tempOut1 $tempOut2

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

label="grib_to_json_test"
REDIRECT=/dev/null
tempOut=temp.$label.txt

if [ ! -x "${tools_dir}/grib_to_json" ]; then
    exit 0
fi

# Currently only works for GRIB edition 1 !!!
${tools_dir}/grib_to_json $ECCODES_SAMPLES_PATH/GRIB1.tmpl > $tempOut

# Decide if we have the JSON verifier commandline utility
JSON_VERIF="json_xs"
JSON_CHECK=""
if command -v $JSON_VERIF >/dev/null 2>&1; then
    JSON_CHECK=$JSON_VERIF
fi

# Check the json_xs command actually works!
set +e
echo '[]' | json_xs > $REDIRECT 2>&1
if [ $? -ne 0 ]; then JSON_CHECK=""; fi
set -e
echo "Using $JSON_CHECK ..."

if test "x$JSON_CHECK" != "x"; then
    json_xs < $tempOut >$REDIRECT 2> $REDIRECT
fi

# Clean up
rm -f $tempOut

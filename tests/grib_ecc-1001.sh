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
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1001
# grib_ls -j -l command (JSON for nearest)
# ---------------------------------------------------------
label="grib_ecc-1001-test"
tempOut=temp.${label}.out

# Decide if we have the JSON verifier commandline utility
JSON_VERIF="json_xs"
JSON_CHECK=""
if command -v $JSON_VERIF >/dev/null 2>&1; then
  JSON_CHECK=$JSON_VERIF
fi

inputs="
  $ECCODES_SAMPLES_PATH/GRIB1.tmpl
  $ECCODES_SAMPLES_PATH/GRIB2.tmpl
  ${data_dir}/high_level_api.grib2
"

for f in $inputs; do
    rm -f $tempOut
    ${tools_dir}/grib_ls -m -j -l 51.0,1.0,1 $f > $tempOut
    if test "x$JSON_CHECK" != "x"; then
        $JSON_VERIF -t none < $tempOut
    fi
done


# Clean up
rm -f $tempOut

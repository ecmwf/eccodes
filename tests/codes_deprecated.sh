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

REDIRECT=/dev/null
label="codes_deprecated_test"
tempFilt="temp.$label.filt"
tempGrib="temp.$label.grib"
tempOut="temp.$label.txt"
tempRef="temp.$label.ref"

input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

echo "Deprecated while statement"
# -----------------------------------------
cat >$tempFilt <<EOF
  while (edition < 2) { print "woo"; }
EOF
set +e
${tools_dir}/grib_filter $tempFilt $input > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "statement is deprecated" $tempOut


echo "Deprecated trigger statement"
# -----------------------------------------
cat >$tempFilt <<EOF
  trigger (edition) { print "woo"; }
EOF
set +e
${tools_dir}/grib_filter $tempFilt $input > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "statement is deprecated" $tempOut


# Clean up
rm -f $tempGrib $tempFilt $tempOut $tempRef

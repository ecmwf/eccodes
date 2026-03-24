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
# This is the test for JIRA issue ECC-2192
# Better error message when comparing strings
# ---------------------------------------------------------

label="codes_ecc-2192_test"
tempFilt=temp.$label.filt
tempOut=temp.$label.txt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat >$tempFilt<<EOF
   if (identifier == "GRIB") { print "matched"; }
EOF
set +e
${tools_dir}/grib_filter $tempFilt $sample_grib2 >$tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
# cat $tempOut
grep -q "Hint: To compare strings.*use.*operator" $tempOut

# Clean up
rm -f $tempFilt $tempOut

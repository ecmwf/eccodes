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

label="grib_filter_fail_test"
tempFilt="temp.$label.filt"
tempGrib="temp.$label.grib"
tempOut="temp.$label.txt"
tempRef="temp.$label.ref"

echo "Test with nonexistent keys. Note spelling of centre!"
# ---------------------------------------------------------
cat >${data_dir}/nonexkey.rules <<EOF
 set center="john";
EOF
# Invoke without -f i.e. should fail if error encountered
set +e
${tools_dir}/grib_filter ${data_dir}/nonexkey.rules ${data_dir}/tigge_pf_ecmwf.grib2 2> $REDIRECT > $REDIRECT
if [ $? -eq 0 ]; then
   echo "grib_filter should have failed if key not found" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}/grib_filter -f ${data_dir}/nonexkey.rules ${data_dir}/tigge_pf_ecmwf.grib2 2> $REDIRECT > $REDIRECT

rm -f ${data_dir}/nonexkey.rules

echo "Test IEEE float overflow"
# -----------------------------------------
input="${samp_dir}/GRIB2.tmpl"
cat >$tempFilt <<EOF
  set values={ 5.4e100 };
  write;
EOF
set +e
${tools_dir}/grib_filter $tempFilt $input 2> $tempOut
status=$?
set -e
[ $status -ne 0 ]
grep -q "ECCODES ERROR.*Number is too large" $tempOut

# Assert statement
# -----------------
cat >$tempFilt <<EOF
  assert(edition == 0);
EOF
set +e
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl 2> $tempOut
status=$?
set -e
[ $status -ne 0 ]
grep "Assertion failure" $tempOut


# Bad write
# ---------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

set +e
echo 'write(-10);' | ${tools_dir}/grib_filter -o $tempGrib - $input > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid argument" $tempOut

# Bad write
# ----------
set +e
echo 'write "/";' | ${tools_dir}/grib_filter - $input > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Unable to open file" $tempOut

# Signed bits
# -----------
cat >$tempFilt <<EOF
  meta _sb signed_bits(widthOfWidths, numberOfGroups);
  print "[_sb]";
EOF
set +e
${tools_dir}/grib_filter $tempFilt $data_dir/boustrophedonic.grib1 > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]


# Non existent filter
# --------------------
set +e
${tools_dir}/grib_filter a_non_existent_filter_file $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Cannot include file" $tempOut


# Clean up
rm -f $tempGrib $tempFilt $tempOut $tempRef

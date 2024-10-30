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

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl


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
${tools_dir}/grib_filter $tempFilt $sample_grib2 2> $tempOut
status=$?
set -e
[ $status -ne 0 ]
grep "Assertion failure" $tempOut


# Bad write
# ---------
set +e
echo 'write(-10);' | ${tools_dir}/grib_filter -o $tempGrib - $sample_grib2 > $tempOut 2>&1
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


# Bad print
# ----------
set +e
echo 'print ("/") "should fail";' | ${tools_dir}/grib_filter - $input > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "IO ERROR" $tempOut


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
${tools_dir}/grib_filter a_non_existent_filter_file $sample_grib2 > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Cannot include file" $tempOut

# Functor not implemented
# -------------------------
cat >$tempFilt <<EOF
  if (unicorn(pl) == 1) { print "Say what?"; }
EOF
set +e
${tools_dir}/grib_filter $tempFilt $sample_grib2 > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "ECCODES ERROR.*failed for 'unicorn'" $tempOut
grep -q "Function not yet implemented" $tempOut

# Direction Increment
# --------------------
input="${samp_dir}/GRIB1.tmpl"
cat >$tempFilt <<EOF
  set ijDirectionIncrementGiven = 0;
  set Ni = 1;
  set Nj = 1;
  set values = { 0 };
  print "[jDirectionIncrementInDegrees]";
EOF
set +e
${tools_dir}/grib_filter $tempFilt $input 2> $tempOut
status=$?
set -e
[ $status -ne 0 ]
grep -q "Cannot compute lat/lon increments. Not enough points" $tempOut



# Clean up
rm -f $tempGrib $tempFilt $tempOut $tempRef

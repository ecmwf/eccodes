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

label="bufr_filter_fail_test"

cd ${data_dir}/bufr

fBufrTmp=${label}".bufr.tmp"
fRules=${label}.filter
fLog=${label}".log"
tempErr=temp.$label.err

#-----------------------------------------------------------
# Test: with nonexistent keys.
#-----------------------------------------------------------
cat > $fRules <<EOF
 set center="98";   #Here centre is misspelled
EOF

# Invoke without -f i.e. should fail if error encountered
set +e

f="syno_1.bufr"
${tools_dir}/codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_filter should have failed if key not found" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}/codes_bufr_filter -f $fRules $f 2>>$fLog 1>>$fLog

#-----------------------------------------------------------
# Test: with not allowed key values
#-----------------------------------------------------------
cat > $fRules <<EOF
 set centre=1024;  #1024 is out of range (it is 8-bit only)
EOF

# Invoke without -f i.e. should fail if error encountered
set +e
f="syno_1.bufr"
${tools_dir}/codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_filter should have failed if key value is not allowed" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}/codes_bufr_filter -f $fRules $f 2>>$fLog 1>>$fLog


#-----------------------------------------------------------
# Test: with invalid string key
#-----------------------------------------------------------
cat > $fRules <<EOF
 set unexpandedDescriptors={1015};
 set stationOrSiteName="Caesar non supra grammaticos"; # Too long
 set pack=1;
 write;
EOF

set +e
f="$ECCODES_SAMPLES_PATH/BUFR4.tmpl"
${tools_dir}/codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_filter should have failed if string key is invalid" >&2
   exit 1
fi
set -e

# Bad filter
# -----------
set +e
${tools_dir}/bufr_filter a_non_existent_filter_file $ECCODES_SAMPLES_PATH/BUFR4.tmpl > $tempErr 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Cannot include file" $tempErr


# Close
# --------
cat > $fRules <<EOF
 transient abcd = "afile"; 
 close(abcd);
EOF
set +e
${tools_dir}/bufr_filter $fRules $ECCODES_SAMPLES_PATH/BUFR4.tmpl > $tempErr 2>&1
status=$?
set -e
[ $status -ne 0 ]
cat $tempErr
grep -q "Input output problem" $tempErr


#-----------------------------------------------------------
# Test: with invalid operator
#-----------------------------------------------------------
cat > $fRules <<EOF
 set unexpandedDescriptors={ 263000, 1001 };
 set pack=1;
 write;
EOF

f="$ECCODES_SAMPLES_PATH/BUFR4.tmpl"
set +e
${tools_dir}/codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog
status=$?
set -e
[ $status -ne 0 ]
grep -q "unsupported operator 63" $fLog


# ------------------------
# Unreadable message
# ------------------------
cat > $fRules <<EOF
  print "[edition]";
EOF
outfile=temp.$label.out
echo BUFR > $outfile
set +e
${tools_dir}/bufr_filter $fRules $outfile > $tempErr 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "unreadable message" $tempErr
rm -f $outfile


# Clean up
rm -f $fLog $fRules $tempErr

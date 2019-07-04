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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-379 (Also ECC-830)
# BUFR encoding failing when value out of range
# ---------------------------------------------------------
cd ${data_dir}/bufr
label="bufr_ecc_379_test"

tempRules=temp.${label}.filter
tempOut=temp.${label}.bufr
tempText=temp.${label}.text
tempRef=temp.${label}.ref

# --------------------------------------------------------
# Test 1
# --------------------------------------------------------
BufrFile=airs_57.bufr
cat > $tempRules <<EOF
 set unpack=1;
 # Two of the longitude values are out-of-range (1st and 3rd)
 set longitude={500, -172, -400, -170, -169, -168, -168, -167, -167, -166, -166, -165, -164, -164, -164};
 set pack=1;
 write;
EOF

# Expect this to fail as two values are out-of-range
set +e
${tools_dir}/codes_bufr_filter -o $tempOut $tempRules $BufrFile 2>/dev/null
status=$?
set -e
[ $status -ne 0 ]
# Now set environment variable to turn out-of-range values into 'missing'
export ECCODES_BUFR_SET_TO_MISSING_IF_OUT_OF_RANGE=1
${tools_dir}/codes_bufr_filter -o $tempOut $tempRules $BufrFile
unset ECCODES_BUFR_SET_TO_MISSING_IF_OUT_OF_RANGE

#echo 'set unpack=1;print "[longitude]";' | ${tools_dir}/bufr_filter - $BufrFile
#echo 'set unpack=1;print "[longitude]";' | ${tools_dir}/bufr_filter - $tempOut > $tempText
#cat > $tempRef << EOF
#-1e+100 -172.14317 -1e+100 -170.17433 -169.407 -168.9308 -168.49104 -167.98389 
#-166.95331 -166.52921 -166.06108 -165.66135 -164.99666 -164.67058 -164.23764
#EOF
#diff $tempText $tempRef

# --------------------------------------------------------
# Test 2
# --------------------------------------------------------
cat > $tempRules <<EOF
 set unpack=1;
 # All longitude values are out-of-range. This is a constant array
 set longitude={500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500};
 set pack=1;
 write;
EOF
# Expect this to fail as all values are out-of-range
set +e
${tools_dir}/codes_bufr_filter -o $tempOut $tempRules $BufrFile 2>/dev/null
status=$?
set -e
[ $status -ne 0 ]
# Now set environment variable to turn out-of-range values into 'missing'
export ECCODES_BUFR_SET_TO_MISSING_IF_OUT_OF_RANGE=1
${tools_dir}/codes_bufr_filter -o $tempOut $tempRules $BufrFile
unset ECCODES_BUFR_SET_TO_MISSING_IF_OUT_OF_RANGE

# --------------------------------------------------------
# Test 3
# --------------------------------------------------------
BufrFile=airc_144.bufr
cat > $tempRules <<EOF
 set unpack=1;
 set latitude=9999;
 set pack=1;
 write;
EOF

# The latitude is out of range. So we expect this to fail
set +e
${tools_dir}/codes_bufr_filter -o $tempOut $tempRules $BufrFile 2>/dev/null
status=$?
set -e
[ $status -ne 0 ]

export ECCODES_BUFR_SET_TO_MISSING_IF_OUT_OF_RANGE=1
${tools_dir}/codes_bufr_filter -o $tempOut $tempRules $BufrFile
unset ECCODES_BUFR_SET_TO_MISSING_IF_OUT_OF_RANGE


# --------------------------------------------------------
# Test 4: use the key setToMissingIfOutOfRange
# --------------------------------------------------------
BufrFile=airc_144.bufr
cat > $tempRules <<EOF
 set unpack=1;
 set setToMissingIfOutOfRange=1;
 set latitude=9999;
 set pack=1;
 write;
EOF
# Will pass now
${tools_dir}/codes_bufr_filter -o $tempOut $tempRules $BufrFile
echo 'set unpack=1; print "lat is now=[latitude]";' | ${tools_dir}/codes_bufr_filter - $tempOut

# --------------------------------------------------------
# Test 5: set setToMissingIfOutOfRange for one message only
# --------------------------------------------------------
BufrFile=syno_multi.bufr
cat > $tempRules <<EOF
 set unpack=1;
 if (count==2) {set setToMissingIfOutOfRange=1;}
 set latitude=5000;
 set pack=1;
 write;
EOF

# syno_multi has 3 messages but only one will get written out
# and its latitude will be missing
rm -f $tempOut
${tools_dir}/codes_bufr_filter -f -o $tempOut $tempRules $BufrFile

count=`${tools_dir}/bufr_count $BufrFile`
[ $count -eq 3 ]
count=`${tools_dir}/bufr_count $tempOut`
[ $count -eq 1 ]
lat=`${tools_dir}/bufr_get -s unpack=1 -p latitude $tempOut`
[ "$lat" = "MISSING" ]


# ------------------------
rm -rf $tempOut $tempRules $tempText

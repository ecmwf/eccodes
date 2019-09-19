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
# This is the test for the JIRA issue ECC-428
# Decoding compressed BUFR data:
# Option to have constant arrays with multiple repeated values
# rather than a single value
# ---------------------------------------------------------
cd ${data_dir}/bufr
label="bufr_ecc_428_test"

tempRules=temp.${label}.filter
tempText=temp.${label}.text
tempErrs=temp.${label}.errs
tempRef1=temp.${label}.ref1
tempRef2=temp.${label}.ref2

# --------------------------------------------------------
# Test 1
# --------------------------------------------------------
bufrFile=airs_57.bufr  # this has 15 subsets
cat > $tempRules <<EOF
 set unpack=1;
 print "[satelliteIdentifier!0]";
EOF

cat $tempRules
${tools_dir}/bufr_filter $tempRules $bufrFile > $tempText
echo "784" > $tempRef1
diff $tempRef1 $tempText

export ECCODES_BUFR_MULTI_ELEMENT_CONSTANT_ARRAYS=1
${tools_dir}/bufr_filter $tempRules $bufrFile > $tempText
echo "784 784 784 784 784 784 784 784 784 784 784 784 784 784 784" > $tempRef2
diff $tempRef2 $tempText

unset ECCODES_BUFR_MULTI_ELEMENT_CONSTANT_ARRAYS
${tools_dir}/bufr_filter $tempRules $bufrFile > $tempText
diff $tempRef1 $tempText

export ECCODES_BUFR_MULTI_ELEMENT_CONSTANT_ARRAYS=0
${tools_dir}/bufr_filter $tempRules $bufrFile > $tempText
diff $tempRef1 $tempText


# --------------------------------------------------------
# Test 2: percentConfidence
# --------------------------------------------------------
bufrFile=amv2_87.bufr
cat > $tempRules <<EOF
 set unpack=1;
 print "[#1#windDirection->percentConfidence!0]";
EOF
cat $tempRules

export ECCODES_BUFR_MULTI_ELEMENT_CONSTANT_ARRAYS=1
${tools_dir}/bufr_filter $tempRules $bufrFile > $tempText 2>$tempErrs
grep -q '^48 54 59.*91 97' $tempText


# --------------------------------------------------------
# Test 3: string arrays
# --------------------------------------------------------
bufrFile=sentinel1.bufr
cat > $tempRules <<EOF
 set unpack=1;
 print "[stationAcquisition!0]";
EOF
cat $tempRules

export ECCODES_BUFR_MULTI_ELEMENT_CONSTANT_ARRAYS=1
${tools_dir}/bufr_filter $tempRules $bufrFile > $tempText 2>$tempErrs
grep -q '^LBG LBG LBG LBG.*LBG$' $tempText


# --------------------------------------------------------
# Test 4: replication factors
# --------------------------------------------------------
bufrFile=sbu8_206.bufr
cat > $tempRules <<EOF
 set unpack=1;
 print "[delayedDescriptorReplicationFactor!0]";
EOF
cat $tempRules

export ECCODES_BUFR_MULTI_ELEMENT_CONSTANT_ARRAYS=1
${tools_dir}/bufr_filter $tempRules $bufrFile > $tempText 2>$tempErrs
grep -q '^6 6 6.*24 24.*6 6' $tempText

# --------------------------------------------------------
# Test XX: Currently failing
# --------------------------------------------------------
bufrFile=asca_139.bufr
cat > $tempRules <<EOF
 set unpack=1;
 print "[/beamIdentifier=2/backscatter]";
EOF

cat $tempRules
${tools_dir}/bufr_filter $tempRules $bufrFile >/dev/null

echo "TODO: Searching for backscatter ... currently failing"
export ECCODES_BUFR_MULTI_ELEMENT_CONSTANT_ARRAYS=1
${tools_dir}/bufr_filter $tempRules $bufrFile 2>$tempErrs
cat $tempErrs

# Clean up
rm -rf $tempRules $tempText $tempRef1 $tempRef2 $tempErrs

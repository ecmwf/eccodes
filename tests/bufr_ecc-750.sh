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
# This is the test for the JIRA issue ECC-750.
# Encoding values which exceed the maximum allowed
# ---------------------------------------------------------
label="bufr_ecc-750-test"
tempRules=temp.${label}.filter
tempBufr=temp.${label}.bufr

# --------------------------------
# Case 1: airTemperature, width=12
# --------------------------------
input=${data_dir}/bufr/temp_101.bufr
cat > $tempRules <<EOF
 set unpack=1;
 set #1#airTemperature=409.6; # max=409.5
 set pack=1;
 write;
EOF

# Should fail
set +e
${tools_dir}/codes_bufr_filter -o $tempBufr $tempRules $input
status=$?
set -e
[ $status -ne 0 ]

# Test for MISSING
echo 'set unpack=1; set #3#airTemperature=409.5; set pack=1; write;' | ${tools_dir}/codes_bufr_filter -o $tempBufr - $input
temperature=`${tools_dir}/bufr_get -s unpack=1 -p '#3#airTemperature' $tempBufr`
[ "$temperature" = "MISSING" ]

# ---------------------------------
# Case 2: inputDataPresentIndicator
# ---------------------------------
input=${data_dir}/bufr/amv2_87.bufr
cat > $tempRules <<EOF
 set inputDataPresentIndicator= {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1,    2,    0, 0, 0, 1, 1, 
      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1};
 set unexpandedDescriptors={
      310195, 222000, 236000, 101103, 31031, 1031, 1032, 101004, 33007, 222000, 
      237000, 1031, 1032, 101004, 33252, 222000, 237000, 1031, 1032, 101004, 
      33253, 222000, 237000, 1031, 1032, 101004, 33007, 222000, 237000, 1031, 
      1032, 101004, 33252, 222000, 237000, 1031, 1032, 101004, 33253, 222000, 
      237000, 1031, 1032, 101004, 33007, 222000, 237000, 1031, 1032, 101004, 
      33252, 222000, 237000, 1031, 1032, 101004, 33253 };
EOF

# Should fail
set +e
${tools_dir}/codes_bufr_filter $tempRules $input
status=$?
set -e
[ $status -ne 0 ]

# -----------------------------------------------------
# Case 3: inputShortDelayedDescriptorReplicationFactor
# -----------------------------------------------------
#input=${data_dir}/bufr/bssh_178.bufr
#cat > $tempRules <<EOF
#set inputShortDelayedDescriptorReplicationFactor= {
#      1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1};
#EOF
#${tools_dir}/codes_bufr_filter $tempRules $input

rm -f $tempRules $tempBufr

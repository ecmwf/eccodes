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

label="bufr_ecc-1304_test"

tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempOut=temp.$label.out

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# Check no crash if invalid encoding
# -------------------------------------
cat > $tempFilt <<EOF
  # Note: we did not specify inputDataPresentIndicator

  set inputDelayedDescriptorReplicationFactor = {40, 160, 40, 40, 160, 40};
  set unexpandedDescriptors = {
     310022, 301011, 301013, 301021, 304034,  
      112000, 31001, 201131, 202129, 7004,  
      7004, 202000, 201000, 202130, 201138, 
      15020, 201000, 202000, 10002,
      224000,   236000, 101000, 31001, 31031, 1031, 
      1032, 8023, 101000, 31001, 224255 };
  write;
EOF

# Command should fail but not crash
set +e
${tools_dir}/bufr_filter $tempFilt $sample_bufr4 > $tempOut
stat=$?
set -e
echo stat=$stat
[ $stat -ne 0 ]

cat $tempOut

# Check the right error message came out
grep -q "ERROR: Size of bitmap is incorrect" $tempOut


# Clean up
rm -f $tempFilt $tempBufr $tempOut

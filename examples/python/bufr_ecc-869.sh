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

#Define a common label for all the tmp files
label="bufr_ecc_869_test_p"

temp=${label}".tmp.txt"

# The output BUFR filename is hardcoded in the Python script
output="outfile_ecc_869_test.bufr"
$PYTHON $examples_src/bufr_ecc-869.py

# Check the output
${tools_dir}/bufr_dump -p $output

echo 'print "[expandedDescriptors%06d]";' | ${tools_dir}/bufr_filter - $output

# The bitmap should have been correctly applied to the 1st and last elements
${tools_dir}/bufr_filter - $output <<EOF
  set unpack=1;
  assert(marineObservingPlatformIdentifier->percentConfidence==44);
  assert(pressureReducedToMeanSeaLevel->percentConfidence==45);
EOF

# Clean up
rm -f $temp $output

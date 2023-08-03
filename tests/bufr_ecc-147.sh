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

# Define a common label for all the tmp files
label="bufr_ecc-147_test"

tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt

cat > $tempFilt <<EOF
 set unpack=1;
 set relativeHumidity=27;
 set horizontalVisibility=1500;
 set pack=1;
 write;
EOF

input="${data_dir}/bufr/syno_1.bufr"
${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $input
# This part of the test is meant to fail
set +e
${tools_dir}/bufr_compare $tempBufr $input
status=$?
set -e
if [ $status -eq 0 ]; then
  # compare should have failed and returned a non-zero exit code
  exit 1
fi
# Now blacklist the failing keys and it should pass
${tools_dir}/bufr_compare -b relativeHumidity,horizontalVisibility $tempBufr $input

# Clean up
rm -f $tempFilt $tempBufr

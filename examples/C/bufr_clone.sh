#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

# Define a common label for all the tmp files
label="bufr_clone_test_c"

fBufrTmp1=${label}.cloned1.bufr
fBufrTmp2=${label}.cloned2.bufr
rm -f $fBufrTmp1 $fBufrTmp2

# We clone this bufr file
f=${data_dir}/bufr/syno_1.bufr

REDIRECT=/dev/null

# Clone the bufr message and change something
${examples_dir}/c_bufr_clone $f $fBufrTmp1

# There should be 3 messages in the new file
count=`${tools_dir}/bufr_count $fBufrTmp1`
[ $count -eq 3 ]

# Compare the first message to the original. It should be different
${tools_dir}/bufr_copy -w count=1 $fBufrTmp1 $fBufrTmp2
set +e
${tools_dir}/bufr_compare $f $fBufrTmp2

# bufr_compare should have returned 1
if [ $? -eq 0 ]; then
   echo "Error: Clones messages should be different!" >&2
   exit 1
fi
set -e

# Clean up
rm -f $fBufrTmp1 $fBufrTmp2

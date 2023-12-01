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
label="bufr_set_keys_test_c"
REDIRECT=/dev/null
fBufrTmp=${label}.tmp.bufr
rm -f $fBufrTmp

# We check "syno_multi.bufr". The path is hard coded in the example
f=${data_dir}/bufr/syno_multi.bufr

set +e
${examples_dir}/c_bufr_set_keys
status=$?
set -e
[ $status -ne 0 ]

# Run
${examples_dir}/c_bufr_set_keys $fBufrTmp  2> $REDIRECT > $REDIRECT

# Compare modified to the original
set +e
${tools_dir}/bufr_compare $f $fBufrTmp >$REDIRECT 2> $REDIRECT
status=$?
set -e
[ $status -ne 0 ]

# Check if modified has the same number of messages
[ `${tools_dir}/bufr_count $f` -eq `${tools_dir}/bufr_count ${fBufrTmp}` ]

# Clean up
rm -f $fBufrTmp

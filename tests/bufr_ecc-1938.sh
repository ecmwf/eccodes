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

# ---------------------------------------------------------------------
# This is the test for JIRA issue ECC-1938
# BUFR: Dealing with corrupted messages
# ---------------------------------------------------------------------

label="bufr_ecc-1938_test"

tempBufr=temp.$label.bufr
tempLog=temp.$label.log
tempOut=temp.$label.txt

# This has 2 unreadable messages
infile=${data_dir}/bufr/bad.bufr
if [ -f "$infile" ]; then
  count=$( ${tools_dir}/bufr_count -f $infile )
  [ $count -eq 24 ]
  set +e
  ${tools_dir}/bufr_copy $infile $tempBufr
  status=$?
  set -e
  [ $status -ne 0 ]
  count=$( ${tools_dir}/bufr_count $tempBufr )
  [ $count -eq 24 ]
fi

# Clean up
rm -f $tempBufr $tempLog $tempOut

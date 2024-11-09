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

REDIRECT=/dev/null

label="bufr_ecc-1938_test"

tempBufr=temp.$label.bufr
tempLog=temp.$label.log
tempOut=temp.$label.txt

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

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

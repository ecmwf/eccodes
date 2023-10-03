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

cd ${data_dir}/bufr

# Define a common label for all the tmp files
label="bufr_change_edition_test"

tempBufr=temp.${label}".bufr"
tempLog=temp.$label.log

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`

for f in ${bufr_files}; do
  if [ "$f" = "ias1_240.bufr" ]; then continue; fi
  
  # Convert to BUFR edition 4
  ${tools_dir}/bufr_set -s editionNumber=4 $f $tempBufr
  ${tools_dir}/bufr_compare -b edition $f $tempBufr
  
  ed=`${tools_dir}/bufr_get -w count=1 -p editionNumber $tempBufr`
  [ "$ed" = "4" ]
done

# Unsupported editions
# -----------------------
f=$ECCODES_SAMPLES_PATH/BUFR3.tmpl
${tools_dir}/bufr_set -s edition=1 $f $tempBufr
set +e
${tools_dir}/bufr_dump -O $tempBufr > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "unreadable message" $tempLog


# Clean up
rm -f $tempBufr $tempLog

#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

#Define a common label for all the tmp files
label="bufr_dump_samples_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp bufr files
fJsonTmp=${label}".json.tmp"

# Test sample BUFR files
echo "ECCODES_SAMPLES_PATH: \""$ECCODES_SAMPLES_PATH"\" ";
echo "fLog: \""$fLog"\" ";
for file in $ECCODES_SAMPLES_PATH/BUFR*.tmpl; do
  ${tools_dir}/bufr_dump -O $file >/dev/null
  ${tools_dir}/bufr_dump -d $file >/dev/null
done

#rm -f $fLog

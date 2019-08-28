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

tempLog=temp.bufr_ls_json.log
rm -f $tempLog

cd ${data_dir}/bufr

# Check JSON validity
# --------------------
# Decide if we have the JSON verifier commandline utility
JSON_VERIF="json_xs"
JSON_CHECK=""
if command -v $JSON_VERIF >/dev/null 2>&1; then
  JSON_CHECK=$JSON_VERIF
fi

bufr_files=`cat bufr_data_files.txt`
for file in ${bufr_files}
do
  ${tools_dir}/bufr_ls -j $file > $tempLog
  if test "x$JSON_CHECK" != "x"; then
    # TODO: Keys like typicalDate/rdbtimeDate can have
    #       values like 00121102 or 000000.
    # JSON check will fail: malformed number (leading zero must not be followed by another digit)
    cat $tempLog | sed '/: 0[0-9]/d' | json_xs -t none
  fi
done


# Clean up
rm -f $tempLog

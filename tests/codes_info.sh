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

label="codes_info_test"
tempLog=temp.$label.log

${tools_dir}/codes_info > $tempLog
grep -q "ecCodes Version" $tempLog

# Check output is nonzero
result=`${tools_dir}/codes_info -v`
[ -n "$result" ]
result=`${tools_dir}/codes_info -s`
[ -n "$result" ]

# Invalid option
set +e
${tools_dir}/codes_info -X > $tempLog 2>&1
status=$?
set -e
[ $status -eq 1 ]


# Clean up
rm -f $tempLog

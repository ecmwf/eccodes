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

label="definitions_test"
REDIRECT=/dev/null
tempOut=temp.$label.txt

[ -z "$ECCODES_DEFINITION_PATH" ] || ECCODES_DEFINITION_PATH=`${tools_dir}/codes_info -d`

for file in `find ${ECCODES_DEFINITION_PATH}/ -name '*.def' -print | grep -v grib3/`
do
  ${tools_dir}/codes_parser $file > $REDIRECT
done

# Try an invalid input
set +e
echo 'transient xx=1' | ${tools_dir}/codes_parser - 2>$tempOut
status=$?
set -e
[ $status -ne 0 ]
grep -q "Parser: syntax error" $tempOut

rm -f $tempOut

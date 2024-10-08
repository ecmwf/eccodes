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

# Ignore symbolic links
# Ignore unsupported formats e.g., TAF, METAR
for file in `find ${ECCODES_DEFINITION_PATH}/ -name '*.def' -type f -print | grep -v grib3/ | grep -v metar/ | grep -v taf/`
do
  ${bin_dir}/codes_parser $file > $REDIRECT
done

# Try an invalid input
set +e
echo 'transient xx=1' | ${bin_dir}/codes_parser - 2>$tempOut
status=$?
set -e
[ $status -ne 0 ]
grep -q "Parser: syntax error" $tempOut

# Clean up
rm -f $tempOut

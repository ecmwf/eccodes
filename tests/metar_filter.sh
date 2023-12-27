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

label="metar_filter_test"

tempFilt="temp.$label.filt"
tempMetar="temp.$label.metar"
tempOut="temp.$label.txt"
tempRef="temp.$label.ref"

input=${data_dir}/metar/metar.txt

cat > $tempFilt <<EOF
  print "[CCCC]";
EOF
${tools_dir}/metar_filter $tempFilt $input


# Bad filter
set +e
${tools_dir}/metar_filter a_non_existent_filter_file $input > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Cannot include file" $tempOut


# Clean up
rm -f $tempMetar $tempFilt $tempOut $tempRef

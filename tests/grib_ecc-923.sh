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
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-923.
# Mismatched count not reported as difference by grib_compare
# ---------------------------------------------------------
label="grib_ecc-923-test"
tempOut=temp.${label}.out
tempGrib1=temp.1.${label}.grib
tempGrib2=temp.2.${label}.grib
tempGrib3=temp.3.${label}.grib

input=${data_dir}/high_level_api.grib2
${tools_dir}/grib_copy -w count=1     $input $tempGrib1
${tools_dir}/grib_copy -w count=1/2   $input $tempGrib2
${tools_dir}/grib_copy -w count=1/2/3 $input $tempGrib3

# 1st file has fewer messages than 2nd file
set +e
${tools_dir}/grib_compare $tempGrib1 $tempGrib2
status=$?
set -e
[ $status -eq 1 ]

# 2nd file has fewer messages than 1st file
set +e
${tools_dir}/grib_compare $tempGrib2 $tempGrib1
status=$?
set -e
[ $status -eq 1 ]



# Clean up
rm -f $tempGrib1 $tempGrib2 $tempGrib3

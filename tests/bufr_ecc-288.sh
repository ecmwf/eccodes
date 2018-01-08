#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-288
# It tests bufr_compare with a relative tolerance
# ---------------------------------------------------------
cd ${data_dir}/bufr
label="bufr_ecc_288_test"

tempRules=temp.${label}.filter
tempOut=temp.${label}.out
BufrFile=syno_1.bufr

cat > $tempRules <<EOF
 set localLongitude=151.831;
 write;
EOF

${tools_dir}/codes_bufr_filter -o $tempOut $tempRules $BufrFile

# There is a difference in localLongitude, rel error=6.58627e-06
# So this should fail
set +e
${tools_dir}/bufr_compare $tempOut $BufrFile
status=$?
set -e
[ $status -eq 1 ]

# Now apply the option and now it should pass
${tools_dir}/bufr_compare -R localLongitude=6.59e-06  $tempOut $BufrFile

rm -rf $tempOut $tempRules

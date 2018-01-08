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
# This is the test for the JIRA issue ECC-286.
# It tests setting a key which starts with a digit
# ---------------------------------------------------------
cd ${data_dir}/bufr
label="bufr_ecc_286_test"

tempRules=temp.${label}.filter
tempOut=temp.${label}.out
tempRef=temp.${label}.ref
BufrFile=syno_1.bufr

cat > $tempRules <<EOF
 set unpack=1;
 print "[3HourPressureChange]";
 set 3HourPressureChange=21;
 print "[3HourPressureChange]";
EOF

${tools_dir}/codes_bufr_filter $tempRules $BufrFile > $tempOut

cat > $tempRef <<EOF
20
21
EOF

diff $tempRef $tempOut

rm -rf $tempOut $tempRef $tempRules

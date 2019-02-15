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
# This is the test for the JIRA issue ECC-393
# It tests bufr_compare with a blacklist with rank
# ---------------------------------------------------------
cd ${data_dir}/bufr
label="bufr_ecc_393_test"

tempRules=temp.${label}.filter
tempOut=temp.${label}.bufr
tempText=temp.${label}.txt
tempRef=temp.${label}.ref
BufrFile=aaen_55.bufr

cat > $tempRules <<EOF
 set unpack=1;
 set #3#channelQualityFlagsForAtovs=1;
 set #5#channelQualityFlagsForAtovs=2;
 set pack=1;
 write;
EOF

${tools_dir}/codes_bufr_filter -o $tempOut $tempRules $BufrFile

# There are two differences. So this should fail
set +e
${tools_dir}/bufr_compare $tempOut $BufrFile
status=$?
set -e
[ $status -eq 1 ]

# Blacklist all the channelQualityFlagsForAtovs keys
${tools_dir}/bufr_compare -b channelQualityFlagsForAtovs $tempOut $BufrFile

# Blacklist both the channelQualityFlagsForAtovs keys individually
${tools_dir}/bufr_compare -b '#3#channelQualityFlagsForAtovs,#5#channelQualityFlagsForAtovs' $tempOut $BufrFile

# Blacklist only one of the channelQualityFlagsForAtovs keys. Will fail
set +e
${tools_dir}/bufr_compare -b '#5#channelQualityFlagsForAtovs' $tempOut $BufrFile >$tempText
status=$?
set -e
[ $status -eq 1 ]

cat > $tempRef <<EOF
== 1 == DIFFERENCE == long [#3#channelQualityFlagsForAtovs]: [1] != [0]
EOF
diff $tempRef $tempText

# Clean
rm -rf $tempOut $tempRules $tempRef $tempText

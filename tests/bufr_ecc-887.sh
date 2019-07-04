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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-887
# BUFR: Search by condition does not work for floating point value
# ---------------------------------------------------------
label="bufr_ecc-887-test"
tempOut=temp.${label}.out
tempRules=temp.${label}.filter


input=${data_dir}/bufr/temp_101.bufr
cat > $tempRules <<EOF
 set unpack=1;
 print "[/airTemperature=272.1/windDirection]";
EOF

res=`${tools_dir}/codes_bufr_filter $tempRules $input`
[ "$res" = "355 2147483647" ]

# Clean up
rm -f $tempRules

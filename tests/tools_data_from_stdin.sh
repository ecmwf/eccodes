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

input=${data_dir}/bufr/syno_multi.bufr

# Test bufr_count
# ---------------
count=`cat $input | ${tools_dir}/bufr_count -`
[ $count -eq 3 ]

# Test bufr_ls
#--------------
input=${data_dir}/bufr/aaen_55.bufr
cat $input | ${tools_dir}/bufr_ls -
result1=`${tools_dir}/bufr_get -p numberOfSubsets $input`
result2=`cat $input | ${tools_dir}/bufr_get -p numberOfSubsets -`
[ "$result1" = "$result2" ]


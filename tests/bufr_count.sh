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

# Enter data dir
cd ${data_dir}/bufr
input=syno_multi.bufr

# Counting valid messages
# ------------------------
count=`${tools_dir}/bufr_count $input`
[ "$count" = "3" ]
count=`${tools_dir}/codes_count $input`
[ "$count" = "3" ]

# Files with invalid (unreadable) messages
# -----------------------------------------
temp=$input.truncated
# BUFR file size = 660 bytes. Write out only the first 640 bytes
# thereby creating an unreadable final message
head -c 640 $input > $temp

set +e
# Without -f, bufr_count should fail
${tools_dir}/bufr_count $temp
status=$?
set -e
[ $status -ne 0 ]

# With -f should count the valid messages
vcount=`${tools_dir}/bufr_count -f $temp`
[ "$vcount" = "2" ]

# Reading from stdin
count2=`cat $input | ${tools_dir}/bufr_count -`
[ $count -eq $count2 ]

rm -f $temp

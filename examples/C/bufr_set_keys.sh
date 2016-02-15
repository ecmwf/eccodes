#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh


#Define a common label for all the tmp files
label="bufr_set_keys_test_c"

#Define tmp file
fBufrTmp=${label}.tmp.bufr
rm -f $fBufrTmp | true

#We check "syno_multi.bufr". The path is
#hardcoded in the example
f=${data_dir}/bufr/syno_multi.bufr

REDIRECT=/dev/null

#
${examples_dir}c_bufr_set_keys $fBufrTmp  2> $REDIRECT > $REDIRECT

#Compare modified to the original
set +e
${tools_dir}/bufr_compare $f $fBufrTmp >$REDIRECT 2> $REDIRECT 

#Check if modified is different
if [ $? -eq 0 ]; then
   echo "setting keys produced identical files " >&2
   exit 1
fi

set -e

#Check if modified has the same number of messages
[ `${tools_dir}bufr_count $f` -eq `${tools_dir}/bufr_count ${fBufrTmp}` ]

#Clean up
rm -f $fBufrTmp | true

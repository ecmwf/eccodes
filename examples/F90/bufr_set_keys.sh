#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

#Define a common label for all the tmp files
label="bufr_set_keys_test_f"

#Prepare tmp file
fBufrTmp=${label}.tmp.bufr
rm -f $fBufrTmp

#The  bufr file to change
f=${data_dir}/bufr/syno_multi.bufr

REDIRECT=/dev/null

#The input ($f) and output ($fBufrTmp) are hardcoded in the f90 example!!!
${examples_dir}/eccodes_f_bufr_set_keys >$REDIRECT

#Compare modified file to the original
set +e
${tools_dir}/bufr_compare $f $fBufrTmp >$REDIRECT

#Check if they are different
if [ $? -eq 0 ]; then
   echo "setting keys produced identical files " >&2
   exit 1
fi

set -e

#Check if modified file has the same number of messages
[ `${tools_dir}/bufr_count $f` = `${tools_dir}/bufr_count ${fBufrTmp}` ]

#Clean up
rm -f ${fBufrTmp}

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
label="bufr_dump_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp bufr files
fJsonTmp=${label}".json.tmp"

#==============================================
# Testing bufr_dump -O
#==============================================
bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
REDIRECT=/dev/null

for file in ${bufr_files}
do
  ${tools_dir}bufr_dump -O ${data_dir}/bufr/$file >/dev/null
done

#==============================================
# Testing output when ECCODES_DEBUG is enabled
#==============================================
file="aaen_55.bufr"
export ECCODES_DEBUG=1

# By default debug output goes to stderr
${tools_dir}bufr_dump -O ${data_dir}/bufr/$file 2>&1 | grep -q "BUFR data .*ing"

# Redirect it to stdout
export ECCODES_LOG_STREAM=stdout
${tools_dir}bufr_dump -O ${data_dir}/bufr/$file | grep -q "BUFR data .*ing"

unset ECCODES_DEBUG
unset ECCODES_LOG_STREAM

#==============================================
# Testing a malformed bufr file (see ECC-110)
#==============================================
# find another way to test malformed bufr 
#echo "Test: malformed bufr file " >> $fLog


#rm -f $fJsonTmp | true

#fBufr=${data_dir}/bufr/"bad.bufr"
# See ECC-276
# In v0.16.0 we changed the shortNames so we use a newer ref file
#fJsonRef=${data_dir}/bufr/"bad.bufr.json.v0.16.0.ref"

#${tools_dir}bufr_dump -js $fBufr > $fJsonTmp

#diff $fJsonTmp $fJsonRef

#==============================================
# Testing change of scale (see ECC-111)
#==============================================
# FIND another way to test this operator
#echo "Test: operator 207003 " >> $fLog

#rm -f $fJsonTmp | true

#fBufr=${data_dir}/bufr/"207003.bufr"
#fJsonRef=${data_dir}/bufr/"207003.bufr.json.ref"
#
#${tools_dir}bufr_dump -ja $fBufr > $fJsonTmp
#
#diff $fJsonTmp $fJsonRef
#
#Clean up
#rm -f $fLog 
#rm -f $fJsonTmp | true

rm -f $fLog

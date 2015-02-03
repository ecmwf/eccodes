#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

#set -x

#Enter data dir
cd ${data_dir}/bufr

#Define a common label for all the tmp files
label="bufr_compare_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp bufr file
fBufrTmp=${label}".bufr.tmp"
fBufrInput1=${label}".bufr.input1"
fBufrInput2=${label}".bufr.input2"

#----------------------------------------------------
# Test: comparing same files
#----------------------------------------------------

f="syno_1.bufr"
echo "Test: comparing the same files" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_compare $f $f >> $fLog

#----------------------------------------------------
# Test: comparing two completely different files
#----------------------------------------------------

#TODO: enable this test when ECC-42 is fixed

set +e

f1="syno_1.bufr"
f2="aaen_55.bufr"
echo "Test: comparing two completely different files" >> $fLog
echo "file: $f" >> $fLog
#${tools_dir}/bufr_compare $f1 $f2 >> $fLog

#if [ $? -eq 0 ]; then
#   echo "bufr_compare should have failed if files are completely different" >&2
#   exit 1
#fi

set -e

#----------------------------------------------------
# Test: comparing with and witout the -b switch
#----------------------------------------------------

#TODO: enable this test when ECC-42 is fixed

f="syno_1.bufr"
echo "Test: comparing with and witout the -b switch" >> $fLog
echo "file: $f" >> $fLog

#Alter a key in the file 
${tools_dir}/bufr_set -s centre=222 $f ${fBufrTmp} >> $fLog

#TODO: enable this test when ECC-42 is fixed

set +e
#${tools_dir}/bufr_compare $f ${fBufrTmp}>> $fLog

#if [ $? -eq 0 ]; then
#   echo "bufr_compare should have failed if files are different" >&2
#   exit 1
#fi

set -e

#Now compare with -b switch. No difference should be found.
${tools_dir}/bufr_compare -b centre $f ${fBufrTmp}>> $fLog

#----------------------------------------------------
# Test: comparing with the -r switch
#----------------------------------------------------

#TODO: enable this test when ECC-42 is fixed

#Create a bufr file with various message types
cat syno_multi.bufr temp_101.bufr > $fBufrInput1 
cat temp_101.bufr syno_multi.bufr > $fBufrInput2

set +e
#${tools_dir}/bufr_compare {fBufrInput1} ${fBufrInput2} >> $fLog

#if [ $? -eq 0 ]; then
#   echo "bufr_compare should have failed if the message order in the files is different" >&2
#   exit 1
#fi

${tools_dir}/bufr_compare -r ${fBufrInput1} ${fBufrInput2}>> $fLog

#Clean up
rm -f $fLog 
rm -f $fBufrTmp | true
rm -f $fBufrInput1 | true
rm -f $fBufrInput2 | true



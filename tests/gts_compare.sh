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

#set -x

#Enter data dir
cd ${data_dir}/gts

#Define a common label for all the tmp files
label="gts_compare_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp GTS file
fGtsTmp=${label}".gts.tmp"
fRules=${label}".filt"
fBufrInput1=${label}".bufr.input1"
fBufrInput2=${label}".bufr.input2"

#----------------------------------------------------
# Test: comparing same files
#----------------------------------------------------
gts_file="EGRR20150317121020_00493212.DAT"
echo "Test: comparing the same files" >> $fLog
echo "file: $gts_file" >> $fLog
${tools_dir}//gts_compare $gts_file $gts_file

#----------------------------------------------------
# Test: comparing two different files
#----------------------------------------------------
cat > $fRules<<EOF
 if (count == 1) { set GG="01"; }
 write;
EOF
${tools_dir}/gts_filter -o $fGtsTmp $fRules $gts_file
set +e
${tools_dir}//gts_compare $gts_file $fGtsTmp
status=$?
set -e
if [ $status -eq 0 ]; then
   echo "gts_compare should have failed if files are different" >&2
   exit 1
fi

#----------------------------------------------------
# Test: comparing with and witout the -b switch
#----------------------------------------------------
${tools_dir}//gts_compare -b GG $gts_file $fGtsTmp >> $fLog

#Clean up
rm -f $fLog $fGtsTmp $fRules

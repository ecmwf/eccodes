#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

# Enter data dir
cd ${data_dir}/metar

# Define a common label for all the tmp files
label="metar_compare_test"

# Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

# Define tmp METAR file
fMetarTmp=${label}".metar.tmp"

#----------------------------------------------------
# Test: comparing same files
#----------------------------------------------------
metar_file="metar.txt"
echo "Test: comparing the same files" >> $fLog
echo "file: $metar_file" >> $fLog
${tools_dir}/metar_compare $metar_file $metar_file

#----------------------------------------------------
# Test: comparing two different files
#----------------------------------------------------
sed -e 's:^METAR VECC 022350Z 00000KT 1600 BR NSC 15/13 Q1013 NOSIG:METAR VECC 022349Z 00000KT 1600 BR NSC 15/13 Q1013 NOSIG:' < metar.txt > $fMetarTmp
set +e
${tools_dir}/metar_compare -v -d $metar_file $fMetarTmp
status=$?
set -e
if [ $status -eq 0 ]; then
   echo "metar_compare should have failed if files are different" >&2
   exit 1
fi

# The -d option should have created these files
rm -f error1_1.metar error2_1.metar error1_2.metar error2_2.metar

#----------------------------------------------------
# Test: comparing with and without the -b switch
#----------------------------------------------------
if [ $ECCODES_ON_WINDOWS -eq 0 ]; then
   # Add wrong blocklist. Should still fail
   set +e
   ${tools_dir}/metar_compare -b CCCC $metar_file $fMetarTmp
   status=$?
   set -e
   [ $status -eq 1 ]
   # Add correct blocklist
   ${tools_dir}/metar_compare -b minute,theMessage $metar_file $fMetarTmp
fi

# Clean up
rm -f $fLog $fMetarTmp

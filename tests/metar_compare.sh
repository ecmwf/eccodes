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
# Test: comparing with skip
#----------------------------------------------------
metar_file="metar.txt"
${tools_dir}/metar_compare -w CCCC=VILK $metar_file $metar_file

#----------------------------------------------------
# Test: comparing two different files
#----------------------------------------------------
sed -e 's:^METAR VECC 022350Z 00000KT 1600 BR NSC 15/13 Q1013 NOSIG:METAR VECC 022349Z 00000KT 1600 BR NSC 15/13 Q1013 NOSIG:' < metar.txt > $fMetarTmp
set +e
${tools_dir}/metar_compare -v -d -f $metar_file $fMetarTmp
status=$?
set -e
if [ $status -eq 0 ]; then
   echo "metar_compare should have failed if files are different" >&2
   exit 1
fi

# Compare using a namespace
set +e
${tools_dir}/metar_compare -c ls:n $metar_file $fMetarTmp > $fLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "DIFFERENCE == string.*dateTime" $fLog

set +e
${tools_dir}/metar_compare -a -c ls:n $metar_file $fMetarTmp > $fLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "DIFFERENCE == string.*dateTime" $fLog
grep -q "DIFFERENCE == string.*theMessage" $fLog


# The -d option should have created these files
rm -f error1_1.metar error2_1.metar error1_2.metar error2_2.metar

#----------------------------------------------------
# Compare a key of type double
#----------------------------------------------------
temp1=temp.$label.metar.1
temp2=temp.$label.metar.2
${tools_dir}/metar_copy -w count=1 $metar_file $temp1
${tools_dir}/metar_copy -w count=2 $metar_file $temp2
# absolute diff. = 16.53, relative diff. = 0.381315
${tools_dir}/metar_compare -c latitude -R latitude=0.4 $temp1 $temp2
${tools_dir}/metar_compare -c latitude -A 17 $temp1 $temp2
rm -f $temp1 $temp2

#----------------------------------------------------
# Comparing with and without the -b switch
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

#----------------------------------------------------
# Compare doubles
#----------------------------------------------------
temp1=temp.$label.1.metar
temp2=temp.$label.2.metar
echo 'METAR LQMO 022350Z 09003KT 6000 FEW010 SCT035 BKN060 09/09 Q1003=' > $temp1
${tools_dir}/metar_copy -w count=1 $metar_file $temp2
set +e
${tools_dir}/metar_compare -b theMessage -f -v $temp1 $temp2 > $fLog
status=$?
set -e
[ $status -ne 0 ]
grep -q "temperature .*1 different" $fLog
grep -q "dewPointTemperature .*1 different" $fLog

# dewPointTemperature diffs: absolute diff. = 1, relative diff. = 0.111111
${tools_dir}/metar_compare -b temperature,theMessage -R dewPointTemperature=0.12 $temp1 $temp2

rm -f $temp1 $temp2

#----------------------------------------------------
# Test with file of the same name in a dir
#----------------------------------------------------
tempDir=temp.$label.dir
rm -fr $tempDir
mkdir $tempDir
cp $metar_file $tempDir
${tools_dir}/metar_compare $metar_file $tempDir
rm -r $tempDir

# Non-existence
set +e
${tools_dir}/metar_compare non-exist1 non-exist2 > $fLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "No such file or directory" $fLog

# Options
set +e
${tools_dir}/metar_compare -a $metar_file $metar_file > $fLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "option requires" $fLog


# Clean up
rm -f $fLog $fMetarTmp

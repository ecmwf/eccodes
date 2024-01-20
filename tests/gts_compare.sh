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
cd ${data_dir}/gts

# Define a common label for all the tmp files
label="gts_compare_test"

fLog=${label}".log"
rm -f $fLog
touch $fLog

fGtsTmp=${label}".gts.tmp"
fRules=${label}".filt"

#----------------------------------------------------
# Test: comparing same files
#----------------------------------------------------
gts_file="EGRR20150317121020_00493212.DAT"
echo "Test: comparing the same files" >> $fLog
echo "file: $gts_file" >> $fLog
${tools_dir}/gts_compare $gts_file $gts_file

#----------------------------------------------------
# Test: comparing with skip
#----------------------------------------------------
gts_file="EGRR20150317121020_00493212.DAT"
${tools_dir}/gts_compare -w TT=SA $gts_file $gts_file

#----------------------------------------------------
# Test: comparing two different files
#----------------------------------------------------
cat > $fRules<<EOF
 if (count == 1) { set GG="01"; }
 write;
EOF
${tools_dir}/gts_filter -o $fGtsTmp $fRules $gts_file
set +e
${tools_dir}/gts_compare -v -d -f $gts_file $fGtsTmp
status=$?
set -e
if [ $status -eq 0 ]; then
   echo "gts_compare should have failed if files are different" >&2
   exit 1
fi
# The -d option should have created these two files
[ -f error1_1.gts ]
[ -f error2_1.gts ]
rm -f error1_1.gts error2_1.gts

#----------------------------------------------------
# Test: comparing with and without the -b switch
#----------------------------------------------------
# Add wrong blocklist. Should still fail
set +e
${tools_dir}/gts_compare -b CCCC $gts_file $fGtsTmp
status=$?
set -e
[ $status -eq 1 ]
# Add correct blocklist
${tools_dir}/gts_compare -b GG $gts_file $fGtsTmp

#----------------------------------------------------
# Compare using -c
#----------------------------------------------------
temp1=temp.$label.1.gts
temp2=temp.$label.2.gts
# Pick two messages which do have different contents
${tools_dir}/gts_copy -w count=1 $gts_file $temp1
${tools_dir}/gts_copy -w count=4 $gts_file $temp2
${tools_dir}/gts_compare -c theMessage $temp1 $temp2
${tools_dir}/gts_compare -c theMessage -a $temp1 $temp2

set +e
${tools_dir}/gts_compare -c ls:n $temp1 $temp2
status=$?
set -e
[ $status -eq 1 ]

set +e
${tools_dir}/gts_compare -c ls:n -a $temp1 $temp2
status=$?
set -e
[ $status -eq 1 ]

rm -f $temp1 $temp2

#----------------------------------------------------
# Test with file of the same name in a dir
#----------------------------------------------------
tempDir=temp.$label.dir
rm -fr $tempDir
mkdir $tempDir
cp $gts_file $tempDir
${tools_dir}/gts_compare $gts_file $tempDir
rm -r $tempDir

# Options
set +e
${tools_dir}/gts_compare -a $gts_file $gts_file > $fLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "option requires" $fLog


# Non-existence
set +e
${tools_dir}/gts_compare non-exist1 non-exist2 > $fLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "No such file or directory" $fLog


# Clean up
rm -f $fLog $fGtsTmp $fRules

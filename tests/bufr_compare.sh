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

#Define filter rules file
fRules=${label}.filter

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
set +e
f1="syno_1.bufr"
f2="aaen_55.bufr"
echo "Test: comparing two completely different files" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_compare $f1 $f2 >> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_compare should have failed if files are completely different" >&2
   exit 1
fi
set -e

#----------------------------------------------------
# Test: comparing with and without the -b switch
#----------------------------------------------------
f="syno_1.bufr"
echo "Test: comparing with and witout the -b switch" >> $fLog
echo "file: $f" >> $fLog

#Alter a key in the file 
${tools_dir}/bufr_set -s dataCategory=2 $f ${fBufrTmp} >> $fLog

set +e
${tools_dir}/bufr_compare $f ${fBufrTmp}>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_compare should have failed if files are different" >&2
   exit 1
fi
set -e

# Now compare with -b switch. No difference should be found.
${tools_dir}/bufr_compare -b dataCategory $f ${fBufrTmp}>> $fLog

#----------------------------------------------------
# Test: comparing with the -r switch
#----------------------------------------------------
#Create a bufr file with various message types
#cat syno_multi.bufr temp_101.bufr > $fBufrInput1 
#cat temp_101.bufr syno_multi.bufr > $fBufrInput2

#set +e
#${tools_dir}/bufr_compare ${fBufrInput1} ${fBufrInput2} >> $fLog
#if [ $? -eq 0 ]; then
#   echo "bufr_compare should have failed if the message order in the files is different" >&2
#   exit 1
#fi
#set -e
#${tools_dir}/bufr_compare -r ${fBufrInput1} ${fBufrInput2}>> $fLog

#----------------------------------------------------
# Change subCentre and compare
#----------------------------------------------------
${tools_dir}/bufr_set -s bufrHeaderSubCentre=12 aaen_55.bufr $fBufrTmp
set +e
${tools_dir}/bufr_compare aaen_55.bufr $fBufrTmp > $fLog 2>&1
status=$?
set -e
[ $status -eq 1 ]
fgrep -q "[bufrHeaderSubCentre]: [70] != [12]" $fLog

#----------------------------------------------------
# Second argument of bufr_compare is a directory
#----------------------------------------------------
temp_dir=tempdir.${label}
mkdir -p $temp_dir
infile=aaen_55.bufr
cp $infile $temp_dir
${tools_dir}/bufr_compare $infile $temp_dir >/dev/null
rm -fr $temp_dir

#----------------------------------------------------
# Compare attributes
#----------------------------------------------------
set +e
${tools_dir}/bufr_compare amv2_87.bufr amv3_87.bufr > $fLog 2>&1
status=$?
set -e
[ $status -eq 1 ]
grep -q "#1#pressure->percentConfidence" $fLog
grep -q "#1#windDirection->percentConfidence" $fLog
grep -q "#1#windSpeed->percentConfidence" $fLog
grep -q "#1#coldestClusterTemperature->percentConfidence" $fLog

#----------------------------------------------------
# Header only mode
#----------------------------------------------------
f="syno_1.bufr"
cat > $fRules <<EOF
 set unpack=1;
 set relativeHumidity=27;
 set horizontalVisibility=1500;
 set pack=1;
 write;
EOF
${tools_dir}/codes_bufr_filter -o $fBufrTmp $fRules $f
# Header keys have not changed
${tools_dir}/bufr_compare -H $f $fBufrTmp

#----------------------------------------------------
# Compare two-way (symmetric mode)
#----------------------------------------------------
f=$ECCODES_SAMPLES_PATH/BUFR3.tmpl
# Add a local section
${tools_dir}/bufr_set -s section2Present=1 $f $fBufrTmp
# Compare A with B will pass
${tools_dir}/bufr_compare $f $fBufrTmp
# Compare with -2 option
set +e
${tools_dir}/bufr_compare -2 $f $fBufrTmp > $fLog 2>&1
status=$?
set -e
[ $status -eq 1 ]

#----------------------------------------------------
# ECC-656: using relative comparison (-R) with 'all'
#----------------------------------------------------
f='airc_142.bufr'
echo 'set unpack=1;set airTemperature=228; set height=1.037e+04; set pack=1; write;' |\
    ${tools_dir}/codes_bufr_filter -o $fBufrTmp - $f
${tools_dir}/bufr_compare -R airTemperature=0.004,height=0.001 $f $fBufrTmp
${tools_dir}/bufr_compare -R all=0.004 $f $fBufrTmp

#----------------------------------------------------
# ECC-658: apply relative comparison (-R) to all
# ranks of a given key
#----------------------------------------------------
f='PraticaTemp.bufr'
${tools_dir}/codes_bufr_filter -o $fBufrTmp - $f <<EOF
 set unpack=1;
 set #1#airTemperature=288.41;
 set #2#airTemperature=286.15;
 set #3#airTemperature=280.95;
 set #4#airTemperature=280.32;
 set #5#airTemperature=280.43;
 set pack=1;
 write;
EOF
# The relative differences are around 3.5e-5. Suppress all instances
${tools_dir}/bufr_compare -R airTemperature=4e-5 $f $fBufrTmp

# Clean up
# -------------
rm -f $fLog $fBufrTmp $fBufrInput1 $fBufrInput2 $fRules

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
cd ${data_dir}/bufr

# Define a common label for all the tmp files
label="bufr_set_test"

# Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

# Define tmp bufr file
fBufrTmp=${label}".bufr.tmp"

#-----------------------------------------------
# Test: setting header for single message file
#-----------------------------------------------
rm -f $fBufrTmp

f="syno_1.bufr"
echo "Test: setting header for single message file" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_set -v -p bufrHeaderCentre,bufrHeaderCentre:l -s bufrHeaderCentre=222 $f $fBufrTmp >> $fLog

centre=`${tools_dir}/bufr_get -p bufrHeaderCentre $fBufrTmp`
[ $centre = "222" ]

#----------------------------------------------------
# Test: setting header for multi-message file
#----------------------------------------------------
rm -f $fBufrTmp

f="syno_multi.bufr"
echo "Test: setting header for multi-message file" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_set -v -p bufrHeaderCentre,bufrHeaderCentre:l -s bufrHeaderCentre=222 $f $fBufrTmp >> $fLog

for i in 1 2 3 ;do
    centre=`${tools_dir}/bufr_get -w count=$i -p bufrHeaderCentre $fBufrTmp`
    [ $centre = "222" ]
done

# Strict option
f=aeolus_wmo_26.bufr
${tools_dir}/bufr_set -S -w localNumberOfObservations=40 -s rdbType=3 $f $fBufrTmp
cnt=$( ${tools_dir}/bufr_count $fBufrTmp )
[ $cnt -eq 3 ]

#-----------------------------------------------------
# Test: setting data values for single message file
#-----------------------------------------------------
# TODO: when ECC-37 is fixed we need to enable it.

rm -f $fBufrTmp

f="syno_1.bufr"
echo "Test: setting data values" >> $fLog
echo "file: $f" >> $fLog
#${tools_dir}/bufr_set -v -p airTemperatureAt2M -s airTemperatureAt2M=234.5 $f $fBufrTmp >> $fLog

#t2m=`${tools_dir}/bufr_get -p airTemperatureAt2M $fBufrTmp`
#[ $t2m = "234.5" ]


#----------------------------------------------------
# Test: setting header for multi-message file
#----------------------------------------------------
# TODO: when ECC-37 is fixed we need to enable it.

rm -f $fBufrTmp

f="syno_multi.bufr"
echo "Test: setting data values for multi-message file" >> $fLog
echo "file: $f" >> $fLog
#${tools_dir}/bufr_set -v -p airTemperatureAt2M -s airTemperatureAt2M=234.5 $f $fBufrTmp >> $fLog

#for i in 1 2 3 ;do
#    centre=`${tools_dir}/bufr_get -F%.1f -w count=$i -p airTemperatureAt2M $fBufrTmp`
#   [ $centre = "234.5" ]
#done

#-----------------------------------------------------------
# Test: No keys set
#-----------------------------------------------------------
set +e
${tools_dir}/bufr_set $f $fBufrTmp > $fLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "provide some keys to set" $fLog

#-----------------------------------------------------------
# Test: with nonexistent keys
#-----------------------------------------------------------
# Key "center" does not exist!!

# Invoke without -f i.e. should fail if error encountered
set +e
f="syno_1.bufr"
echo "Test: nonexistent keys" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_set -s center=98 $f $fBufrTmp 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_set should have failed if key not found" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}/bufr_set -f -s center=98 $f $fBufrTmp 2>>$fLog 1>>$fLog


#-----------------------------------------------------------
# Test: with not allowed key values
#-----------------------------------------------------------
# Here 1024 is out of range for centre (it is 8-bit only for edition=3 files)

# Invoke without -f i.e. should fail if error encountered
set +e
f="syno_1.bufr"
echo "Test: nonexistent keys" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_set -s bufrHeaderCentre=1024 $f $fBufrTmp 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_set should have failed if key not found" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}/bufr_set -f -s bufrHeaderCentre=1024 -f $f $fBufrTmp 2>>$fLog 1>>$fLog

#-----------------------------------------------------------
# Test: key values out of range
#-----------------------------------------------------------
f=aaen_55.bufr

# The correction1 key is of type "bits" and only 6 bits wide
# So its range is 0 -> 63 inclusive
${tools_dir}/bufr_set -s correction1=63 $f $fBufrTmp 2>>$fLog 1>>$fLog

set +e
${tools_dir}/bufr_set -s correction1=65 $f $fBufrTmp 2>>$fLog 1>>$fLog
if [ $? -eq 0 ]; then
   echo "bufr_set should have failed if value too large" >&2
   exit 1
fi
${tools_dir}/bufr_set -s correction1=-1 $f $fBufrTmp 2>>$fLog 1>>$fLog
if [ $? -eq 0 ]; then
   echo "bufr_set should have failed if value negative" >&2
   exit 1
fi
set -e

#-----------------------------------------------------------
# Test: Local ECMWF section. The 'ident' key
#-----------------------------------------------------------
f=temp_101.bufr
${tools_dir}/bufr_set -s ident=ABCD $f $fBufrTmp
result=`${tools_dir}/bufr_get -p ident $fBufrTmp`
[ "$result" = "ABCD" ]
${tools_dir}/bufr_set -s keyMore=ABCD $f $fBufrTmp
result=`${tools_dir}/bufr_get -p keyMore,ident $fBufrTmp`
[ "$result" = "ABCD ABCD" ]

${tools_dir}/bufr_set -s ident=' AB CD ' $f $fBufrTmp
result=`${tools_dir}/bufr_get -p ident $fBufrTmp`
[ "$result" = "AB CD" ]

# ${tools_dir}/bufr_compare $f $fBufrTmp


#-----------------------------------------------------------
# ECC-1359: string that can be converted to an integer
# ----------------------------------------------------------
sample=$ECCODES_SAMPLES_PATH/BUFR4_local.tmpl
${tools_dir}/bufr_set -s messageLength:s=333 $sample $fBufrTmp
result=`${tools_dir}/bufr_get -p messageLength $fBufrTmp`
[ "$result" = "333" ]

#-----------------------------------------------------------
# Invalid masterTablesVersionNumber
#-----------------------------------------------------------
sample=$ECCODES_SAMPLES_PATH/BUFR4.tmpl
${tools_dir}/bufr_set -s masterTablesVersionNumber=255 $sample $fBufrTmp
set +e
${tools_dir}/bufr_dump -p $fBufrTmp 2>>$fLog 1>>$fLog
if [ $? -eq 0 ]; then
   echo "bufr_dump should have failed" >&2
   exit 1
fi
set -e
grep -q "unable to find definition file sequence.def.*bufr/tables/0/local/0/98/0/sequence.def" $fLog
grep -q "ECCODES ERROR.*unable to get hash value for sequences" $fLog

# ECC-1739
sample=$ECCODES_SAMPLES_PATH/BUFR3.tmpl
${tools_dir}/bufr_set -s masterTablesVersionNumber=255,localTablesVersionNumber=1 $sample $fBufrTmp
set +e
${tools_dir}/bufr_dump -p $fBufrTmp >$fLog 2>&1
status=$?
set -e
grep -q "ECCODES ERROR.*unable to get hash value for sequences" $fLog


# Unreadable message
#-----------------------------------------------------------
echo BUFR > $fBufrTmp
set +e
${tools_dir}/bufr_set -s masterTablesVersionNumber=10 $fBufrTmp /dev/null > $fLog 2>&1
status=$?
set -e
grep -q "unreadable message" $fLog


# Clean up
rm -f $fLog 
rm -f $fBufrTmp

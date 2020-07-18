#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh


# Enter data dir
cd ${data_dir}/bufr

# Define a common label for all the tmp files
label="bufr_copy_test"

# Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

# Define tmp bufr files
fBufrInput=${label}".bufr.input"
fBufrTmp=${label}".bufr.tmp"

# Create a bufr file with various message types
cat syno_multi.bufr temp_101.bufr > $fBufrInput 

#----------------------------------------------------
# Test: copy synop messages
#----------------------------------------------------
rm -f $fBufrTmp

echo "Test: copy synop messages " >> $fLog
${tools_dir}/bufr_copy -w dataCategory=0 $fBufrInput $fBufrTmp >> $fLog

for i in 1 2 3 ;do
    category=`${tools_dir}/bufr_get -w count=$i -p dataCategory:l $fBufrTmp`
    [ $category = "0" ]
done

#----------------------------------------------------
# Test: copy non-synop messages
#----------------------------------------------------
rm -f $fBufrTmp

echo "Test: copy non-synop messages " >> $fLog
${tools_dir}/bufr_copy -w dataCategory!=0 $fBufrInput $fBufrTmp >> $fLog

[ `${tools_dir}/bufr_get -p dataCategory:l $fBufrTmp`  = "2" ]

#-------------------------------------------------------------------
# Test: use the square brackets to insert the value of a key
#-------------------------------------------------------------------
rm -f ${fBufrTmp}
rm -f ${fBufrTmp}_*.bufr

echo "Test: use the square brackets to insert the value of a key " >> $fLog
${tools_dir}/bufr_copy $fBufrInput ${fBufrTmp}_[dataCategory].bufr >> $fLog

[ -s ${fBufrTmp}_0.bufr ]
[ -s ${fBufrTmp}_2.bufr ]

rm -f ${fBufrTmp}_*.bufr
rm -f $fBufrInput

#-------------------------------------------------------------------
# Test: The -X option
#-------------------------------------------------------------------
echo "Test: use of -X option" >> $fLog
fBufrInput=aeolus_wmo_26.bufr
${tools_dir}/bufr_copy -w count=1 -X 0 $fBufrInput $fBufrTmp #First msg
r1=`${tools_dir}/bufr_get -w count=1 -n ls $fBufrInput`
r2=`${tools_dir}/bufr_get -n ls $fBufrTmp`
[ "$r1" = "$r2" ]

#${tools_dir}/bufr_copy -w count=1 -X 19449 $fBufrInput $fBufrTmp #Last msg
#r1=`${tools_dir}/bufr_get -w count=11 -n ls $fBufrInput`
#r2=`${tools_dir}/bufr_get -n ls $fBufrTmp`
#[ "$r1" = "$r2" ]

#${tools_dir}/bufr_copy -w count=1 -X 10972 $fBufrInput $fBufrTmp
#r=`${tools_dir}/bufr_get -p typicalTime,numberOfSubsets $fBufrTmp`
#[ "$r" = "000013 41" ]

# Clean up
#-----------
rm -f $fLog 
rm -f $fBufrTmp

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
label="bufr_set_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp bufr file
fBufrTmp=${label}".bufr.tmp"

#----------------------------------------------------
# Test: setting header for single message file
#----------------------------------------------------

rm -f $fBufrTmp | true

f="syno_1.bufr"
echo "Test: setting header for single message file" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_set -v -p centre,centre:l -s centre=222 $f $fBufrTmp >> $fLog

centre=`${tools_dir}/bufr_get -p centre $fBufrTmp`
[ $centre = "222" ]

#----------------------------------------------------
# Test: setting header for multi-message file
#----------------------------------------------------

rm -f $fBufrTmp | true

f="syno_multi.bufr"
echo "Test: setting header for multi-message file" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_set -v -p centre,centre:l -s centre=222 $f $fBufrTmp >> $fLog

for i in 1 2 3 ;do
    centre=`${tools_dir}/bufr_get -w count=$i -p centre $fBufrTmp`
    [ $centre = "222" ]
done


#-----------------------------------------------------
# Test: setting data values for single message file
#-----------------------------------------------------

#TODO: when ECC-37 is fixed we need to enable it.

rm -f $fBufrTmp | true

f="syno_1.bufr"
echo "Test: setting data values" >> $fLog
echo "file: $f" >> $fLog
#${tools_dir}/bufr_set -v -p airTemperatureAt2M -s airTemperatureAt2M=234.5 $f $fBufrTmp >> $fLog

#t2m=`${tools_dir}/bufr_get -p airTemperatureAt2M $fBufrTmp`
#[ $t2m = "234.5" ]


#----------------------------------------------------
# Test: setting header for multi-message file
#----------------------------------------------------

#TODO: when ECC-37 is fixed we need to enable it.

rm -f $fBufrTmp | true

f="syno_multi.bufr"
echo "Test: setting data values for multi-message file" >> $fLog
echo "file: $f" >> $fLog
#${tools_dir}/bufr_set -v -p airTemperatureAt2M -s airTemperatureAt2M=234.5 $f $fBufrTmp >> $fLog

#for i in 1 2 3 ;do
#    centre=`${tools_dir}/bufr_get -F%.1f -w count=$i -p airTemperatureAt2M $fBufrTmp`
#   [ $centre = "234.5" ]
#done


#Clean up
rm -f $fLog 
rm -f $fBufrTmp | true




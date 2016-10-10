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

#set -x

#Enter data dir
cd ${data_dir}/bufr

#Define a common label for all the tmp files
label="bufr_get_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp file
fTmp=${label}".tmp.txt"
rm -f $fTmp

#Define another tmp file to store the test results
res_get=${label}".get.test"
rm -f $res_get

#-------------------------------------------
# Test "-p" switch
#-------------------------------------------
f="aaen_55.bufr"

#The reference is the same as for ls
ref_get=$f".ls.ref"

echo "Test: -p switch" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_get -p totalLength,bufrHeaderCentre,bufrHeaderSubCentre,masterTableNumber,masterTablesVersionNumber,localTablesVersionNumber,numberOfSubsets,localNumberOfObservations $f > $fTmp

#Write the values into a file and compare with ref
cat $fTmp | awk '{split($0,a," "); for (i=1; i<=8; i++) print a[i]}' > $res_get
diff $ref_get $res_get


#-------------------------------------------
# ECC-236
#-------------------------------------------
result=`${tools_dir}/bufr_get -s unpack=1 -p nonCoordinatePressure syno_1.bufr`
[ "$result" = "100910" ]
result=`${tools_dir}/bufr_get -s unpack=1 -p stationOrSiteName bssh_176.bufr`
[ "$result" = "Helgoland" ]
result=`${tools_dir}/bufr_get -s unpack=1 -p majorFrameCount aaen_55.bufr`
[ "$result" = "MISSING" ]
result=`${tools_dir}/bufr_get -s unpack=1 -p satelliteIdentifier wavb_134.bufr`
[ "$result" = "MISSING" ]


#Clean up
rm -f $fLog $fTmp $res_get

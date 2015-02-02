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

#Create log file
fLog="bufr_filter.log"
rm -f $fLog
touch $fLog

#Create split directory
[ -d split ] || mkdir -p split 

#Define filter file
fRules="bufr_filter.filter"

#-----------------------------------------------------------
# Filter out only header information that all
# the bufr files must have. We just check if it works.
#-----------------------------------------------------------

echo "Test: dump header"

cat > $fRules <<EOF
print "[centre] [subCentre] [masterTablesVersionNumber] [localTablesVersionNumber] [numberOfSubsets]"; 
EOF

for f in `ls *.bufr` ; do
   echo "file: $f" >> $fLog
   ${tools_dir}/bufr_filter $fRules $f >> $fLog
done

#-----------------------------------------------------------
# Test: dump SYNOP values
#-----------------------------------------------------------

cat > $fRules <<EOF
set unpack=1;
transient statid=1000*blockNumber+stationNumber;
print "statid=[statid] lat=[latitude] lon=[longitude] t2=[airTemperatureAt2M]";
EOF

f="syno_multi.bufr"
echo "Test: dump SYNOP values"
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fRules $f >> $fLog

#-----------------------------------------------------------
# Test: filter SYNOP message according to conditions
#-----------------------------------------------------------

#Filter out the message with stationid=1003
cat > $fRules <<EOF
set unpack=1;
transient statid=1000*blockNumber+stationNumber;

if (statid == 1003) {
	write "res_[statid]";
}		
EOF

fBufrTmp="res_1003"
rm -f $fBufrTmp | true

f="syno_multi.bufr"
echo "Test: filter SYNOP message according to conditions"
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fRules $f >> $fLog

#Check if the resulting bufr message is the right one
cat > $fRules <<EOF
set unpack=1;
transient statid=1000*blockNumber+stationNumber;
print "[statid]";
EOF

[ `${tools_dir}/bufr_filter $fRules $fBufrTmp` = "1003" ] 

#-----------------------------------------------------------
# Test: splitting according to keys 
#-----------------------------------------------------------

cat > $fRules <<EOF 
set unpack=1;
write "split/split_[centre]_[masterTablesVersion]_[localTablesVersion]_[1000*blockNumber+stationNumber].bufr";
EOF

[ -d split ] || mkdir -p split 

f="syno_multi.bufr"
echo "Test: splitting according to keys"
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fRules $f >> $fLog

#Check if the resulting files exist
for statid  in 1001 1003 1007 ; do
    [ -s split/split_98_13_1_${statid}.bufr ]
done

#Clean up
rm -f split/*
rm -f $fLog $fRules $fBufrTmp




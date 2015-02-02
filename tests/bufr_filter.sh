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

#Enter data dir
cd ${data_dir}/bufr

fLog="bufr_filter.log"
rm -f $fLog

fTmp="tmp.bufr_filter.txt"
rm -f $fTmp

#Create log file
touch $fLog

#Define filter file
fFilter="bufr_filter.filter"

#-----------------------------------------------------------
# Filter out only header information the all
# the bufr files must have. We just check if it works.
#-----------------------------------------------------------

cat > $fFilter <<EOF
print "[centre] [subCentre] [masterTablesVersionNumber] [localTablesVersionNumber] [numberOfSubsets]"; 
EOF

for f in `ls *.bufr` ; do
   echo "file: $f" >> $fLog
   ${tools_dir}/bufr_filter $fFilter $f >> $fLog
done

#-----------------------------------------------------------
# SYNOP values tests
#-----------------------------------------------------------

cat > $fFilter <<EOF
set unpack=1;
transient statid=1000*blockNumber+stationNumber;
print "statid=[statid] lat=[latitude] lon=[longitude] t2=[airTemperatureAt2M]";
EOF

f="syno_multi.bufr"
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fFilter $f >> $fLog

#-----------------------------------------------------------
# SYNOP message filter tests
#-----------------------------------------------------------

cat > $fFilter <<EOF
set unpack=1;
transient statid=1000*blockNumber+stationNumber;

if (statid == 1003) {
	write "res_[statid]";
}		
EOF

fBufrTmp="res_1003"
rm -f $fBufrTmp | true

f="syno_multi.bufr"
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fFilter $f >> $fLog

cat > $fFilter <<EOF
set unpack=1;
transient statid=1000*blockNumber+stationNumber;
print statid
EOF

[ `${tools_dir}/bufr_filter $fFilter $fBufrTmp` = "1003" ] 


#Clean up
rm -f $fLog $res_ls $fTmp $fFilter $fBufrTmp




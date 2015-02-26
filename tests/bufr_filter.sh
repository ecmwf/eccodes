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
label="bufr_filter_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Create split directory
dSplit=${label}"_split"
[ -d $dSplit ] || mkdir -p $dSplit 

#Define tmp bufr file
fBufrTmp=${label}".bufr.tmp"

#Define filter rules file
fRules=${label}.filter

#-----------------------------------------------------------
# Filter out only header information that all
# the bufr files must have. We just check if it works.
#-----------------------------------------------------------

echo "Test: dump header" >> $fLog

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
echo "Test: dump SYNOP values" >> $fLog
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
	write "${fBufrTmp}";
}		
EOF

rm -f $fBufrTmp | true

f="syno_multi.bufr"
echo "Test: filter SYNOP message according to conditions" >> $fLog
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

#TODO: when ECC-32 is fixed we need to remove hack using the transient variables!

cat > $fRules <<EOF
set unpack=1;
transient centre_tmp=centre;
transient block_tmp=blockNumber;
transient station_tmp=stationNumber;
#write "$dSplit}/split_[centre]_[masterTablesVersionNumber]_[localTablesVersionNumber]_[blockNumber]_[stationNumber].bufr";
write "${dSplit}/split_[centre_tmp]_[masterTablesVersionNumber]_[localTablesVersionNumber]_[block_tmp]_[station_tmp].bufr";
EOF

f="syno_multi.bufr"
echo "Test: splitting according to keys" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fRules $f >> $fLog

#Check if the resulting files exist
for statid  in 1 3 7 ; do
    [ -s ${dSplit}/split_98_13_1_1_${statid}.bufr ]
done

#-----------------------------------------------------------
# Test:  print attributes
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "pressure=[pressure] [pressure->units]";
print "pressure->code=[pressure->code!06d]";
print "pressure->scale=[pressure->scale]";
print "pressure->reference=[pressure->reference]";
print "pressure->width=[pressure->width]";
print "pressure->percentConfidence=[pressure->percentConfidence] [pressure->percentConfidence->units]";
print "pressure->percentConfidence->code=[pressure->percentConfidence->code!06d]";
print "pressure->percentConfidence->scale=[pressure->percentConfidence->scale]";
print "pressure->percentConfidence->reference=[pressure->percentConfidence->reference]";
print "pressure->percentConfidence->width=[pressure->percentConfidence->width]";
EOF

f="syno_1.bufr"
echo "Test: attributes" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}/bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
pressure=100910 Pa
pressure->code=010004
pressure->scale=-1
pressure->reference=0
pressure->width=14
pressure->percentConfidence=74 %
pressure->percentConfidence->code=033007
pressure->percentConfidence->scale=0
pressure->percentConfidence->reference=0
pressure->percentConfidence->width=7
EOF

diff ${f}.ref ${f}.log 

rm -f ${f}.ref ${f}.log

#-----------------------------------------------------------
# Test:  access element by rank
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "pressure=[pressure#4] [pressure#4->units]";
print "pressure=[pressure]";
EOF

f="temp_101.bufr"
echo "Test: access element by rank" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}/bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
pressure=98500 Pa
pressure=102000 101800 100000 98500 96400 92500 92100 89700 
88100 86100 85000 84400 79400 79000 78300 77300 
71900 70000 69400 65100 61200 53400 50000 43900 
40000 39900 37800 31600 30000 27500 25000 21200 
21000 20600 20400 20000 19300 18400 17000 16600 
15100 15000 14600 14000 13400 13200 12900 11100 
10800 10000 8960 7630 7000 6420 6190 5770 
5320 5000 3970 3570 3190 3090 3000 2820 
2630 2400 2340 2050 2000 1680 1530 1500 
1380 1300 1210 31600
EOF

diff ${f}.ref ${f}.log 

rm -f ${f}.ref ${f}.log

#-----------------------------------------------------------
# Test:  access marker operators
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "brightnessTemperature={[brightnessTemperature#2!',']}";
print "brightnessTemperature->percentConfidence={[brightnessTemperature#2->percentConfidence!',']}";
print "brightnessTemperature->firstOrderStatisticalValue={[brightnessTemperature#2->firstOrderStatisticalValue!',']}";
EOF

f="b005_89.bufr"
echo "Test: access marker operators" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}/bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
brightnessTemperature={-1e+100,290.8,-1e+100,289.7,289.5,289.5,289.4,287.5,
287.4,288.3,288.2,-1e+100,-1e+100,-1e+100,-1e+100,-1e+100,
288.7,288.8,288.8,289.6,289.9,289,-1e+100,289.9,
289.8,288.8,289,-1e+100,290.7,290.3,291.1,292.3,
292.2,292.7,292.7,292.4,292.5,292.6,292.6,292.2,
291.9,292.1,292,291.8,292.6,292.5,292.7,292.8,
292.3,292,291.8,292.2,292.1,291.8,291.6,291.5,
290.6,290.8,290.5,290.6,290.5,290.3,290.1,290.7,
291.2,289.9,290,290.9,290.7,-1e+100,290.3,290.1,
289.9,289.9,289.8,289.4,289.3,289.3,289.4,289.8,
289.9,289.9,289.9,290,290.5,289.9,290,290.6,
290.6,290.5,290.3,291,291.2,290.9,291.2,291.3,
291.4,291.3,291.6,291.6,291.8,291.4,291.6,291.6,
291.7,291.5,291.4,291.8,291.7,291.6,291.4,291.7,
291.6,291.4,291.4,291.4,-1e+100,291.3,291.3,290.8,
291.8,290.8,291.3,291.9,291.9,292.5,292.6,290.8
}
brightnessTemperature->percentConfidence={4294967295,0,4294967295,55,50,83,83,1,
0,17,0,4294967295,4294967295,4294967295,4294967295,4294967295,
48,16,29,14,11,15,4294967295,39,
21,0,0,4294967295,9,4,20,86,
71,96,96,68,0,0,0,96,
0,95,66,57,96,88,95,0,
97,76,94,90,92,94,89,77,
6,48,10,4,10,23,59,45,
94,3,9,58,64,4294967295,61,84,
85,27,6,47,39,65,0,0,
5,48,3,45,71,26,5,71,
64,67,57,88,90,0,96,98,
99,89,86,97,71,50,55,82,
95,83,51,97,97,86,59,87,
77,46,1,0,4294967295,0,0,0,
0,0,2,0,1,0,1,0
}
brightnessTemperature->firstOrderStatisticalValue={-1e+100,0,-1e+100,0.7,0.6,0.5,0.6,0.3,
0.3,0.7,0.7,-1e+100,-1e+100,-1e+100,-1e+100,-1e+100,
0.6,0.5,0.6,0.6,0.7,0.4,-1e+100,0.8,
0.7,0,0.3,-1e+100,0.5,0.5,0.9,0.5,
0.5,0.2,0.2,0.4,0,0,0,0.4,
0,0.4,0.3,0.6,0.5,0.5,0.4,0,
0.3,0.6,0.5,0.5,0.5,0.5,0.5,0.6,
0.5,0.6,0.4,0.6,0.5,0.4,0.5,0.5,
0.5,0.2,0.4,0.7,0.7,-1e+100,0.5,0.6,
0.6,0.4,0.3,0.3,0.3,0.4,0,0,
0.3,0.3,0.2,0.4,0.6,0.4,0.3,0.7,
0.7,0.7,0.5,0.5,0.5,0,0.4,0.4,
0.3,0.5,0.6,0.3,0.7,0.4,0.3,0.6,
0.5,0.4,0.3,0.4,0.4,0.5,0.5,0.5,
0.4,0.3,0.2,0,-1e+100,0,0,0,
1.7,0,1.4,2.5,2.3,2.6,2.2,1.2
}
EOF

diff ${f}.ref ${f}.log 

rm -f ${f}.ref ${f}.log

#-----------------------------------------------------------
# Test:  access marker operators 2
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "pressure=[pressure]";
print "pressure->percentConfidence=[pressure->percentConfidence]";
print "pressure->width=[pressure->width]";
EOF

f="temp_101.bufr"
echo "Test: access marker operators 2" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}/bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
pressure=102000 101800 100000 98500 96400 92500 92100 89700 
88100 86100 85000 84400 79400 79000 78300 77300 
71900 70000 69400 65100 61200 53400 50000 43900 
40000 39900 37800 31600 30000 27500 25000 21200 
21000 20600 20400 20000 19300 18400 17000 16600 
15100 15000 14600 14000 13400 13200 12900 11100 
10800 10000 8960 7630 7000 6420 6190 5770 
5320 5000 3970 3570 3190 3090 3000 2820 
2630 2400 2340 2050 2000 1680 1530 1500 
1380 1300 1210 31600
pressure->percentConfidence=70 75 82 75 75 82 75 75 
75 75 82 75 75 75 75 75 
75 82 75 75 75 75 82 75 
82 75 75 75 82 75 82 79 
75 75 75 82 75 75 75 75 
75 82 75 75 75 75 75 75 
75 82 75 75 82 75 75 75 
75 82 75 75 75 75 82 75 
75 75 75 75 82 75 75 75 
75 75 75 70
pressure->width=14 14 14 14 14 14 14 14 
14 14 14 14 14 14 14 14 
14 14 14 14 14 14 14 14 
14 14 14 14 14 14 14 14 
14 14 14 14 14 14 14 14 
14 14 14 14 14 14 14 14 
14 14 14 14 14 14 14 14 
14 14 14 14 14 14 14 14 
14 14 14 14 14 14 14 14 
14 14 14 14
EOF

diff ${f}.ref ${f}.log 

rm -f ${f}.ref ${f}.log

#-----------------------------------------------------------
# Test: with nonexistent keys.
#-----------------------------------------------------------

#Here "centre" is misspelled!!!
cat > $fRules <<EOF
set center="98";
EOF

# Invoke without -f i.e. should fail if error encountered
set +e

f="syno_1.bufr"
echo "Test: nonexistent keys" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fRules $f 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_filter should have failed if key not found" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}/bufr_filter -f $fRules $f 2>>$fLog 1>>$fLog


#-----------------------------------------------------------
# Test: with not allowed key values
#-----------------------------------------------------------

#Here 1024 is out of range for centre (it is 8-bit only)
cat > $fRules <<EOF
set centre=1024;
EOF

# Invoke without -f i.e. should fail if error encountered
set +e

f="syno_1.bufr"
echo "Test: not allowed key values" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_filter $fRules $f 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_filter should have failed if key value is not allowed" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}/bufr_filter -f $fRules $f 2>>$fLog 1>>$fLog


#----------------------------------------------------
# Test: format specifier for integer keys
#----------------------------------------------------

#TODO: when ECC-36 is fixed we need to enable the output check again.

cat > $fRules <<EOF
# Pad center with leading zeroes and heightOfStation with blanks
set unpack=1;
print "centre=[centre%.3d], height=[heightOfStation%5ld]";
EOF

f="syno_1.bufr"
echo "Test: nformat specifier for integer keys" >> $fLog
echo "file: $f" >> $fLog
result=`${tools_dir}/bufr_filter  $fRules $f`
#[ "$result" = "centre=098, height=    3" ]


#----------------------------------------------------
# Test: setting keys 
#----------------------------------------------------

#TODO: when ECC-37 is fixed we need to enable it.

#Filter out the message with stationid=1003
cat > $fRules <<EOF
set unpack=1;
set typicalDate="20010511";
set year=2001;
set airTemperatureAt2M=234.5;
EOF

rm -f $fBufrTmp | true

f="syno_1.bufr"
echo "Test: setting keys" >> $fLog
echo "file: $f" >> $fLog
#${tools_dir}/bufr_filter $fRules $f -o $fBufrTmp >> $fLog

#Check if the resulting bufr message is the right one
cat > $fRules <<EOF
set unpack=1;
print "[typicalDate] [year] [airTemperatureAt2M%.1f]";
EOF

#[ `${tools_dir}/bufr_filter $fRules $fBufrTmp` = "20010511 2001 234.5" ]


#Clean up
rm -f ${dSplit}/*
rm -f $fLog $fRules 
rm -f $fBufrTmp | true




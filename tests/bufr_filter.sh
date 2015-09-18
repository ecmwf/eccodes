#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

set -x
. ./include.sh

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

bufr_files=`cat bufr_data_files.txt`
for f in ${bufr_files} ; do
   echo "file: $f" >> $fLog
   ${tools_dir}bufr_filter $fRules $f >> $fLog
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
${tools_dir}bufr_filter $fRules $f >> $fLog

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
${tools_dir}bufr_filter $fRules $f >> $fLog

#Check if the resulting bufr message is the right one
cat > $fRules <<EOF
set unpack=1;
transient statid=1000*blockNumber+stationNumber;
print "[statid]";
EOF

[ `${tools_dir}bufr_filter $fRules $fBufrTmp` = "1003" ] 

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
${tools_dir}bufr_filter $fRules $f >> $fLog

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
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
print "pressure=[#4#pressure] [#4#pressure->units]";
print "pressure=[pressure]";
EOF

f="temp_101.bufr"
echo "Test: access element by rank" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
print "brightnessTemperature={[#2#brightnessTemperature!',']}";
print "brightnessTemperature->percentConfidence={[#2#brightnessTemperature->percentConfidence!',']}";
print "brightnessTemperature->firstOrderStatisticalValue={[#2#brightnessTemperature->firstOrderStatisticalValue!',']}";
EOF

f="b005_89.bufr"
echo "Test: access marker operators" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
# Test:  access by condition
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "/beamIdentifier=2/backscatter=[/beamIdentifier=2/backscatter]";
EOF

f="asca_139.bufr"
echo "Test: access by condition" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
/beamIdentifier=2/backscatter=-24.6 -24.78 -24.92 -25.05 -25.04 -24.72 -23.83 -22.57 
-21.71 -21.76 -21.81 -20.97 -19.97 -19.01 -17.8 -16.22 
-14.67 -13.26 -12.02 -11.01 -9.84 -7.31 -8.76 -10.13 
-11.36 -12.58 -13.49 -13.87 -13.77 -13.44 -13.42 -13.58 
-13.92 -14.6 -15.36 -16.22 -17.11 -17.98 -18.56 -18.58 
-18.49 -18.45 -22.66 -22.99 -23.37 -23.85 -24.27 -24.57 
-24.54 -24.17 -23.96 -24.47 -24.53 -23.11 -21.62 -20.27 
-18.93 -17.42 -15.78 -14.13 -12.6 -11.35 -10.06 -7.38 
-8.57 -9.82 -11.43 -12.88 -13.83 -14.25 -14.21 -14.16 
-14.32 -14.44 -14.73 -15.21 -15.94 -17 -17.87 -18.64 
-19.31 -19.52 -19.49 -19.32 -21.03 -21.35 -21.89 -22.54 
-23.05 -23.63 -24.2 -24.67 -25.29 -26.17 -26.72 -25.84 
-23.76 -21.4 -19.65 -18.37 -16.87 -15.2 -13.55 -12.06 
-10.58 -7.89 -8.96 -10.3 -11.99 -13.2 -14 -14.65 
-14.94 -15.08 -15.41 -15.63 -15.72 -15.86 -16.48 -17.61 
-18.16 -18.4 -18.99 -19.69 -19.96 -19.66 -19.85 -20.1 
-20.6 -21.03 -21.26 -21.69 -22.36 -23 -23.61 -24.14 
-24.65 -25.15 -24.67 -22.61 -20.73 -19.69 -18.63 -16.96 
-15.02 -13.19 -11.39 -8.81 -10.08 -11.44 -12.7 -13.62 
-14.39 -15.05 -15.54 -16.06 -16.61 -16.72 -16.39 -16.03 
-16.23 -16.78 -16.77 -16.88 -17.6 -18.42 -18.91 -18.69 
-19.15 -19.34 -19.64 -19.75 -19.72 -19.93 -20.39 -20.92 
-21.35 -21.57 -21.5 -21.6 -21.86 -21.37 -20.23 -19.44 
-19.13 -18.31 -16.75 -14.67 -12.4 -9.59 -11.09 -12.25 
-13.21 -14.16 -14.96 -15.52 -16.1 -16.9 -17.52 -17.55 
-16.99 -16.1 -15.4 -15.06 -15 -15.41 -16.21 -16.92 
-17.31 -17.15 -19.07 -19.1 -19.07 -18.98 -18.93 -18.97 
-19.13 -19.43 -19.84 -20.11 -19.88 -19.75 -19.94 -19.76 
-18.83 -17.94 -17.58 -17.15 -16.44 -14.97 -12.82 -9.96 
-11.6 -12.72 -13.7 -14.73 -15.51 -16.07 -16.73 -17.52 
-18.03 -17.81 -16.83 -15.45 -14.36 -13.91 -14.13 -14.77 
-15.6 -16.27 -16.45 -16.26 -19.69 -19.74 -19.39 -19.17 
-19.07 -18.92 -18.77 -18.74 -18.9 -19.14 -19.03 -18.89 
-19.18 -19.33 -18.47 -17.42 -16.8 -16.13 -15.33 -14.26 
-12.45 -9.94 -11.43 -12.69 -13.89 -14.97 -15.83 -16.45 
-16.96 -17.36 -17.29 -16.41 -15.13 -14.08 -13.56 -13.51 
-13.87 -14.48 -15.32 -16.01 -16.11 -16.02 -20.39 -20.83 
-20.5 -20.14 -19.7 -19.13 -18.59 -18.05 -17.62 -17.48 
-17.4 -17.47 -18.03 -18.48 -17.84 -16.89 -16.3 -15.7 
-14.82 -13.58 -11.77 -9.62 -10.94 -12.35 -13.56 -14.61 
-15.58 -16.04 -16.12 -16.02 -15.41 -14.46 -13.76 -13.49 
-13.47 -13.62 -13.96 -14.41 -15.14 -15.77 -15.99 -16.05 
-21.2 -21.87 -21.7 -21.21 -20.41 -19.52 -18.84 -18.06 
-17.14 -16.49 -16.18 -16.34 -16.89 -17.24 -16.8 -16.03 
-15.48 -14.93 -14.03 -12.78 -11.25 -9.42 -10.69 -11.98 
-13.04 -13.99 -14.75 -14.93 -14.92 -14.78 -14.24 -13.68 
-13.54 -13.65 -13.73 -13.91 -14.22 -14.69 -15.29 -15.82 
-16.1 -16.25 -22.32 -22.84 -22.24 -21.41 -20.76 -20.11 
-19.55 -18.73 -17.28 -16.02 -15.48 -15.76 -16.39 -16.55 
-15.95 -15.21 -14.7 -14.23 -13.47 -12.46 -11.22 -9.44 
-10.65 -11.82 -12.87 -13.61 -14.05 -14.21 -14.32 -14.25 
-13.98 -13.76 -13.81 -13.97 -14.07 -14.27 -14.62 -15.1 
-15.56 -15.99 -16.38 -16.86 -21.55 -21.83 -21.17 -20.33 
-20.04 -19.78 -19.07 -18.06 -16.75 -15.59 -14.98 -15.03 
-15.48 -15.56 -14.72 -13.86 -13.49 -13.36 -12.93 -11.95 
-10.61 -9.38 -10.6 -11.77 -12.78 -13.4 -13.87 -14.2 
-14.32 -14.19 -14.11 -14.08 -14.1 -14.28 -14.49 -14.78 
-15.17 -15.59 -15.95 -16.31 -16.83 -17.54 -20.78 -20.86 
-20.42 -19.88 -19.85 -19.68 -18.62 -17.29 -16.35 -15.91 
-15.49 -14.96 -14.61 -14.36 -13.68 -12.73 -12.08 -11.71 
-11.28 -10.29 -8.91 -9.25 -10.57 -11.78 -12.63 -13.3 
-13.92 -14.4 -14.71 -14.71 -14.7 -14.69 -14.63 -14.7 
-14.94 -15.33 -15.81 -16.24 -16.49 -16.77 -17.3 -17.87 
-20.68 -20.42 -19.91 -19.75 -20.11 -20.17 -18.91 -17.28 
-16.46 -16.6 -16.61 -15.7 -14.65 -14.01 -13.43 -12.57 
-11.67 -10.91 -10.29 -9.26 -7.86 -9.39 -10.72 -11.83 
-12.54 -13.26 -13.84 -14.36 -14.96 -15.33 -15.46 -15.48 
-15.39 -15.28 -15.41 -15.79 -16.31 -16.74 -16.83 -17.06 
-17.59 -18.11 -20.69 -20.29 -19.68 -19.6 -20.27 -20.84 
-19.82 -18.15 -17.24 -17.3 -17.4 -16.57 -15.37 -14.52 
-13.77 -12.92 -12.05 -11.19 -10.31 -9.14 -7.68 -9.55 
-10.75 -11.81 -12.63 -13.42 -13.91 -14.31 -14.94 -15.52 
-15.86 -15.96 -15.82 -15.69 -15.82 -16.13 -16.5 -16.8 
-16.94 -17.24 -17.79 -18.35 -20.91 -20.45 -19.81 -19.44 
-19.98 -20.72 -20.12 -18.95 -18.28 -18.08 -17.84 -17.3 
-16.39 -15.52 -14.49 -13.48 -12.69 -11.77 -10.52 -9.08 
-7.69 -9.6 -10.75 -11.83 -12.84 -13.64 -14.06 -14.45 
-15.13 -15.82 -16.24 -16.27 -15.95 -15.87 -16.19 -16.57 
-16.77 -16.93 -17.16 -17.6 -18.18 -18.62 -20.82 -20.13 
-19.65 -19.25 -19.53 -19.97 -19.55 -18.86 -18.63 -18.44 
-17.79 -17.32 -16.84 -16.07 -14.94 -13.94 -13.23 -12.3 
-10.82 -9.19 -7.83 -9.63 -10.93 -12.02 -12.92 -13.56 
-14.02 -14.6 -15.46 -16.29 -16.7 -16.61 -16.25 -16.24 
-16.64 -17.1 -17.26 -17.4 -17.71 -18.18 -18.7 -18.99 
-20.59 -19.8 -19.41 -19.23 -19.49 -19.61 -19.22 -18.79 
-18.57 -18.23 -17.45 -16.9 -16.6 -16.08 -15.04 -14.14 
-13.55 -12.74 -11.24 -9.62 -8.23 -9.57 -11.04 -12.15 
-12.9 -13.5 -14.06 -14.68 -15.56 -16.37 -16.83 -16.95 
-16.78 -16.84 -17.2 -17.61 -17.85 -18.05 -18.34 -18.64 
-19.04 -19.28 -20.85 -20.06 -19.67 -19.57 -19.72 -19.43 
-18.91 -18.59 -18.17 -17.6 -17.01 -16.62 -16.35 -15.89 
-15.02 -14.2 -13.59 -12.84 -11.46 -9.94 -8.6 -9.26 
-10.79 -11.87 -12.7 -13.49 -14.14 -14.71 -15.42 -16.08 
-16.72 -17.24 -17.35 -17.48 -17.86 -18.33 -18.68 -18.93 
-19.16 -19.34 -19.56 -19.55 -21.72 -20.86 -20.44 -20.34 
-20.19 -19.46 -18.66 -18.15 -17.69 -17.21 -16.92 -16.69 
-16.27 -15.67 -14.95 -14.11 -13.29 -12.56 -11.41 -9.93 
-8.59 -8.83 -10.29 -11.46 -12.51 -13.43 -14.04 -14.63 
-15.3 -15.93 -16.71 -17.47 -17.87 -18.09 -18.54 -19.16 
-19.62 -19.99 -20.23 -20.3 -20.36 -19.93 -22.54 -21.65 
-21.19 -21.05 -20.68 -19.62 -18.55 -17.87 -17.55 -17.31 
-17.17 -16.98 -16.4 -15.69 -15 -14.08 -13.13 -12.42 
-11.36 -9.9 -8.51 -8.7 -10.02 -11.15 -12.34 -13.37 
-14.02 -14.7 -15.47 -16.17 -16.89 -17.67 -18.33 -18.74 
-19.25 -19.98 -20.59 -21.06 -21.23 -21.17 -21.06 -20.25 
-22.71 -21.9 -21.41 -21.11 -20.59 -19.5 -18.53 -18.04 
-17.93 -17.7 -17.38 -17.18 -16.58 -15.74 -15.06 -14.15 
-13.15 -12.34 -11.25 -9.82 -8.43 -8.86 -9.99 -10.98 
-12.22 -13.41 -14.24 -14.98 -15.84 -16.56 -17.22 -18.01 
-18.78 -19.35 -19.95 -20.72 -21.32 -21.74 -21.92 -22 
-21.81 -20.83 -22.64 -22.06 -21.4 -20.8 -20.13 -19.19 
-18.51 -18.36 -18.33 -17.98 -17.55 -17.26 -16.54 -15.58 
-14.79 -13.84 -12.86 -11.98 -10.88 -9.6 -8.41 -8.96 
-9.9 -10.91 -12.2 -13.36 -14.07 -14.76 -15.73 -16.62 
-17.38 -18.22 -19.02 -19.67 -20.33 -21.05 -21.5 -21.73 
-21.93 -22.16 -22.16 -21.57 -22.38 -22.08 -21.29 -20.51 
-19.75 -18.91 -18.42 -18.31 -18.22 -17.9 -17.57 -17.26 
-16.42 -15.44 -14.59 -13.62 -12.68 -11.82 -10.77 -9.59 
-8.51 -8.91 -9.9 -11.01 -12.15 -13.07 -13.58 -14.16 
-15.22 -16.33 -17.11 -17.92 -18.88 -19.76 -20.52 -21.11 
-21.2 -21.11 -21.18 -21.46 -21.74 -21.83 -21.82 -21.8 
-21.06 -20.28 -19.59 -18.88 -18.48 -18.23 -17.95 -17.68 
-17.5 -17.25 -16.42 -15.44 -14.69 -13.83 -12.89 -11.93 
-10.8 -9.52 -8.41 -8.94 -10.1 -11.2 -12.09 -12.88 
-13.42 -14.01 -15.09 -16.17 -16.71 -17.32 -18.43 -19.68 
-20.57 -21.07 -20.9 -20.54 -20.39 -20.6 -21.15 -21.9 
-21.31 -21.32 -20.78 -20.08 -19.6 -19.13 -18.75 -18.38 
-17.93 -17.59 -17.47 -17.28 -16.58 -15.63 -14.98 -14.22 
-13.25 -12.11 -10.79 -9.41 -8.21 -9.05 -10.42 -11.48 
-12.24 -12.96 -13.53 -14.23 -15.28 -16.05 -16.4 -16.99 
-18.16 -19.51 -20.42 -20.89 -20.69 -20.24 -19.8 -19.8 
-20.48 -21.79 -21.22 -20.9 -20.34 -19.79 -19.47 -19.14 
-18.71 -18.3 -18.03 -17.86 -17.69 -17.4 -16.8 -15.84 
-15.04 -14.25 -13.27 -12.11 -10.72 -9.3 -8.13 -9.07 
-10.57 -11.64 -12.45 -13.12 -13.7 -14.52 -15.5 -15.97 
-16.21 -16.87 -18.01 -19.19 -19.99 -20.43 -20.39 -19.98 
-19.46 -19.43 -20.07 -21.44 -21.51 -20.73 -19.9 -19.48 
-19.24 -18.94 -18.52 -18.11 -18.16 -18.31 -18.02 -17.41 
-16.67 -15.75 -14.77 -13.85 -12.89 -11.82 -10.48 -9.11 
-8.05 -8.82 -10.33 -11.58 -12.57 -13.3 -13.95 -14.88 
-15.83 -16.28 -16.41 -16.99 -18.01 -18.92 -19.49 -19.84 
-19.87 -19.55 -19.35 -19.51 -20.17 -21.42 -21.56 -20.75 
-19.86 -19.47 -19.26 -18.94 -18.45 -18.03 -18.12 -18.26 
-17.88 -17.1 -16.27 -15.44 -14.42 -13.47 -12.57 -11.52 
-10.28 -9.02 -7.93 -8.63 -10.13 -11.37 -12.49 -13.42 
-14.11 -14.93 -15.93 -16.63 -16.86 -17.36 -18.22 -18.95 
-19.41 -19.63 -19.4 -19.16 -19.44 -19.95 -20.65 -21.68 
-21.05 -20.56 -19.86 -19.47 -19.31 -19.01 -18.39 -17.94 
-17.96 -17.93 -17.47 -16.74 -15.94 -15.14 -14.19 -13.37 
-12.61 -11.59 -10.36 -9.12 -7.98 -8.64 -10.07 -11.22 
-12.32 -13.36 -14.05 -14.76 -15.82 -16.65 -17.06 -17.58 
-18.33 -19.14 -19.64 -19.63 -19.09 -18.91 -19.58 -20.51 
-21.26 -21.93 -20.12 -19.88 -19.3 -18.93 -18.82 -18.58 
-18.05 -17.66 -17.63 -17.55 -17.17 -16.48 -15.71 -14.96 
-14.15 -13.5 -12.85 -11.82 -10.57 -9.43 -8.38 -8.95 
-10.28 -11.39 -12.36 -13.25 -13.94 -14.65 -15.64 -16.43 
-17.04 -17.78 -18.5 -19.18 -19.7 -19.72 -19.29 -19.25 
-20.03 -21.15 -21.94 -22.42 -18.93 -18.86 -18.41 -18.03 
-17.84 -17.6 -17.22 -16.91 -16.8 -16.72 -16.61 -16.24 
-15.63 -14.95 -14.24 -13.69 -13.03 -11.94 -10.73 -9.66 
-8.74 -9.57 -10.66 -11.63 -12.6 -13.42 -14.15 -14.94 
-15.72 -16.37 -17.1 -18.04 -18.88 -19.46 -19.85 -19.89 
-19.74 -19.86 -20.6 -21.72 -22.55 -22.87 -17.94 -17.98 
-17.65 -17.3 -17.1 -16.91 -16.65 -16.35 -16.17 -16.04 
-16 -15.99 -15.64 -15.02 -14.38 -13.83 -13.12 -12.07 
-10.89 -9.7 -8.77 -10.03 -11.04 -12.04 -13.13 -14.02 
-14.75 -15.61 -16.31 -16.76 -17.36 -18.37 -19.44 -20.11 
-20.19 -20.01 -20.09 -20.44 -21.18 -22.2 -22.91 -23.02 
-17.49 -17.44 -17.16 -16.96 -16.91 -16.8 -16.6 -16.26 
-15.99 -15.78 -15.71 -15.76 -15.45 -14.9 -14.41 -13.9 
-13.21 -12.16 -10.92 -9.73 -8.8 -9.93 -11.26 -12.49 
-13.66 -14.71 -15.6 -16.45 -17.14 -17.47 -17.95 -18.86 
-19.84 -20.46 -20.32 -19.95 -20.14 -20.87 -21.77 -22.57 
-23.08 -23.14 -17.34 -17.11 -16.75 -16.59 -16.64 -16.68 
-16.59 -16.3 -16 -15.74 -15.65 -15.6 -15.27 -14.78 
-14.38 -14 -13.38 -12.37 -11.13 -9.93 -9.02 -9.6 
-11.08 -12.53 -13.96 -15.27 -16.31 -17 -17.46 -17.74 
-18.11 -18.86 -19.81 -20.49 -20.31 -19.76 -19.69 -20.43 
-21.37 -22.29 -22.98 -23.06 -17.21 -16.95 -16.54 -16.23 
-16.22 -16.37 -16.39 -16.18 -15.98 -15.8 -15.68 -15.55 
-15.29 -14.84 -14.45 -14.08 -13.4 -12.52 -11.43 -10.18 
-9.21 -9.25 -10.6 -12.02 -13.65 -15.16 -16.13 -16.6 
-16.94 -17.23 -17.66 -18.51 -19.5 -20.26 -20.32 -19.79 
-19.43 -19.77 -20.45 -21.39 -22.17 -22.19 -17.09 -16.87 
-16.47 -16.13 -16.09 -16.3 -16.43 -16.21 -16.02 -15.87 
-15.78 -15.69 -15.32 -14.83 -14.46 -13.96 -13.27 -12.43 
-11.35 -10.17 -9.2 -8.97 -10.24 -11.52 -12.99 -14.29 
-15.12 -15.66 -16.16 -16.59 -17.09 -17.84 -18.79 -19.82 
-20.25 -19.85 -19.52 -19.63 -20.02 -20.79 -21.45 -21.5 
-17.01 -16.85 -16.41 -16.06 -16.11 -16.52 -16.88 -16.73 
-16.5 -16.29 -16.15 -15.99 -15.43 -14.84 -14.48 -13.91 
-13.22 -12.38 -11.25 -10.17 -9.2 -8.91 -10.17 -11.29 
-12.47 -13.55 -14.39 -15.02 -15.74 -16.36 -16.73 -17.26 
-18.22 -19.41 -20.03 -19.89 -19.71 -19.64 -19.75 -20.34 
-21.09 -21.27 -16.97 -16.91 -16.41 -15.95 -16.14 -16.9 
-17.53 -17.55 -17.37 -17.04 -16.7 -16.38 -15.72 -15.01 
-14.65 -14.09 -13.31 -12.46 -11.31 -10.22 -9.29 -9.01 
-10.12 -11.16 -12.26 -13.28 -14.12 -14.74 -15.6 -16.34 
-16.61 -17.06 -18.02 -19.14 -19.69 -19.82 -19.7 -19.46 
-19.4 -19.91 -20.64 -21.02 -17.05 -16.98 -16.45 -15.99 
-16.35 -17.31 -17.93 -17.97 -17.85 -17.48 -17.02 -16.72 
-16.09 -15.3 -14.92 -14.37 -13.51 -12.53 -11.25 -10.06 
-9.16 -9.13 -10.16 -11.21 -12.28 -13.27 -14 -14.6 
-15.44 -16.11 -16.46 -17.11 -18.14 -18.87 -19.12 -19.48 
-19.63 -19.51 -19.54 -19.96 -20.4 -20.69 -17.25 -17.07 
-16.54 -16.24 -16.75 -17.62 -17.85 -17.7 -17.57 -17.29 
-17.06 -16.96 -16.43 -15.72 -15.29 -14.74 -13.78 -12.65 
-11.21 -9.83 -8.83 -9.3 -10.33 -11.4 -12.52 -13.55 
-14.08 -14.4 -15.11 -15.9 -16.51 -17.34 -18.15 -18.45 
-18.75 -19.3 -19.65 -19.78 -20.05 -20.48 -20.7 -20.66 
-17.3 -16.95 -16.51 -16.43 -16.89 -17.35 -17.21 -16.88 
-16.75 -16.6 -16.56 -16.68 -16.54 -15.99 -15.49 -14.99 
-14.05 -12.84 -11.28 -9.77 -8.62 -9.49 -10.59 -11.7 
-12.93 -14.07 -14.37 -14.34 -14.91 -15.83 -16.76 -17.73 
-18.21 -18.49 -19.09 -19.54 -19.56 -19.77 -20.37 -20.92 
-21.1 -20.87 -17.17 -16.54 -16.18 -16.28 -16.48 -16.56 
-16.34 -16.12 -16.08 -15.91 -15.73 -15.8 -16 -15.87 
-15.45 -14.94 -14.03 -12.8 -11.31 -9.78 -8.55 -9.49 
-10.86 -12.06 -13.34 -14.46 -14.65 -14.63 -15.18 -15.99 
-17 -18.07 -18.56 -19.11 -19.85 -19.95 -19.6 -19.88 
-20.64 -21.1 -21.09 -20.87 -16.87 -16.07 -15.7 -15.73 
-15.76 -15.71 -15.64 -15.62 -15.57 -15.31 -14.94 -14.84 
-15.09 -15.44 -15.43 -14.86 -13.88 -12.75 -11.4 -9.87 
-8.59 -9.49 -11.09 -12.41 -13.7 -14.67 -14.89 -15.21 
-15.75 -16.29 -17.13 -18.14 -18.91 -19.71 -20.42 -20.34 
-20.12 -20.53 -21.04 -21.2 -21.13 -21.06 -16.37 -15.53 
-15.13 -15.02 -15.09 -15.23 -15.34 -15.33 -15.06 -14.62 
-14.24 -14.06 -14.25 -14.85 -15.37 -14.96 -13.96 -12.9 
-11.67 -10.14 -8.81 -9.42 -11.01 -12.39 -13.72 -14.72 
-15.17 -15.7 -16.13 -16.48 -17.16 -18.09 -18.92 -19.68 
-20.36 -20.54 -20.67 -21.05 -21.13 -21.05 -21.19 -21.41 
-15.93 -15.1 -14.58 -14.32 -14.46 -14.94 -15.36 -15.21 
-14.69 -14.15 -13.82 -13.62 -13.72 -14.35 -15.1 -15.15 
-14.24 -13.13 -12.07 -10.71 -9.31 -9.31 -10.8 -12.18 
-13.46 -14.63 -15.39 -15.98 -16.34 -16.63 -17.17 -17.98 
-18.69 -19.15 -19.64 -20.16 -20.62 -20.91 -20.86 -20.86 
-21.23 -21.53 -15.77 -15.12 -14.54 -14.11 -14.15 -14.69 
-15.36 -15.23 -14.65 -14.17 -13.85 -13.63 -13.7 -14.22 
-14.86 -15.17 -14.57 -13.61 -12.59 -11.24 -9.79 -9.4 
-10.8 -12.1 -13.29 -14.53 -15.48 -16.05 -16.5 -16.92 
-17.34 -17.85 -18.36 -18.75 -19.2 -19.87 -20.51 -20.87 
-21.08 -21.41 -21.77 -21.81 -16.07 -15.58 -15.02 -14.45 
-14.28 -14.69 -15.31 -15.28 -14.85 -14.44 -14.1 -13.9 
-13.91 -14.17 -14.58 -14.97 -14.83 -14.12 -13.04 -11.65 
-10.25 -9.53 -10.86 -12.12 -13.31 -14.48 -15.46 -16.09 
-16.67 -17.15 -17.49 -17.81 -18.23 -18.82 -19.46 -20.13 
-20.73 -21.31 -21.91 -22.32 -22.51 -22.5 -17.07 -16.57 
-15.82 -15.01 -14.72 -15.09 -15.57 -15.55 -15.18 -14.78 
-14.42 -14.16 -14.02 -14.07 -14.22 -14.58 -14.78 -14.39 
-13.43 -12.14 -10.77 -9.61 -11.03 -12.33 -13.52 -14.62 
-15.61 -16.33 -16.92 -17.39 -17.73 -18.1 -18.55 -19.22 
-19.97 -20.6 -21.22 -21.94 -22.49 -22.73 -23.02 -23.36

EOF

diff ${f}.ref ${f}.log 

rm -f ${f}.ref ${f}.log

#-----------------------------------------------------------
# Test:  access by condition 2
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "----- /verticalSoundingSignificance=4/airTemperature -----";
print "[/verticalSoundingSignificance=4/airTemperature]";
print "----- verticalSoundingSignificance -----";
print "[verticalSoundingSignificance]";
print "----- airTemperature -----";
print "[airTemperature]";
EOF

f="temp_101.bufr"
echo "Test: access by condition 2" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
----- /verticalSoundingSignificance=4/airTemperature -----
272.1 269.5 268.1 267.9 266.7 266.1 264.9 264.9 
260.5 260.9 263.5 263.7 261.7 261.9 259.1 258.9 
251.5 243.9 238.3 236.7 221.7 212.7 215.5 215.9 
214.1 217.3 218.3 217.3 219.3 218.9 219.5 217.9 
218.3 217.5 220.3 219.1 220.1 217.3 216.5 217.7 
215.9 217.1 213.5 216.1 214.7 216.1 215.3 216.5 
213.9 215.3 215.7 212.7 214.1 216.1 213.7 215.3 
214.9
----- verticalSoundingSignificance -----
68 4 32 4 4 32 4 4 
4 4 32 4 4 4 4 4 
4 32 4 4 4 4 32 4 
32 4 4 8 32 4 32 20 
4 4 4 32 4 4 4 4 
4 32 4 4 4 4 4 4 
4 32 4 4 32 4 4 4 
4 32 4 4 4 4 32 4 
4 4 4 4 32 4 4 4 
4 4 4 8
----- airTemperature -----
272.1 272.1 270.5 269.5 268.1 267.9 267.9 266.7 
266.1 264.9 264.9 264.9 260.5 260.9 263.5 263.7 
261.7 261.7 261.9 259.1 258.9 251.5 248.9 243.9 
238.5 238.3 236.7 -1e+100 226.1 221.7 218.7 212.7 
212.7 215.5 215.9 215.1 214.1 217.3 218.3 217.3 
219.3 219.1 218.9 219.5 217.9 218.3 217.5 220.3 
219.1 217.9 220.1 217.3 217.5 216.5 217.7 215.9 
217.1 215.5 213.5 216.1 214.7 216.1 216.3 215.3 
216.5 213.9 215.3 215.7 215.1 212.7 214.1 216.1 
213.7 215.3 214.9
EOF

diff ${f}.ref ${f}.log 

rm -f ${f}.ref ${f}.log

#-----------------------------------------------------------
# Test: set unexpandedDescriptors big test
#-----------------------------------------------------------
fRulesReady="set_unexpandedDescriptors.filter"
f="syno_1.bufr"
testScript="set_unexpandedDescriptors_test.sh"
echo "Test: set unexpandedDescriptors big test" >> $fLog
echo "file: $f" >> $fLog

cat >$testScript <<EOF
set -e

EOF
chmod +x $testScript

${tools_dir}bufr_filter $fRulesReady $f 2>> $fLog 1>> $testScript

./$testScript

rm -f new_*bufr 
rm -f $testScript

#-----------------------------------------------------------
# Test:  packing   
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
set pack=1;
write;
EOF

files=" 207003.bufr aaen_55.bufr aben_55.bufr ahws_139.bufr airc_142.bufr airc_144.bufr alws_139.bufr amda_144.bufr
amsa_55.bufr amsb_55.bufr amse_55.bufr amsu_55.bufr amv2_87.bufr amv3_87.bufr asbh_139.bufr asbl_139.bufr
asca_139.bufr asch_139.bufr ascs_139.bufr aseh_139.bufr asel_139.bufr ashs_139.bufr atap_55.bufr ateu_155.bufr
atms_201.bufr atov_55.bufr avhm_87.bufr avhn_87.bufr avhr_58.bufr b002_95.bufr b002_96.bufr b003_56.bufr
b004_145.bufr b005_87.bufr b006_96.bufr b007_31.bufr bssh_178.bufr btem_109.bufr buoy_27.bufr cmwi_87.bufr
cmwn_87.bufr cnow_28.bufr cori_156.bufr crit_202.bufr emsg_87.bufr euwv_87.bufr fy3a_154.bufr fy3b_154.bufr
go15_87.bufr goee_87.bufr goes_87.bufr goga_89.bufr hirb_55.bufr hirs_55.bufr ias1_240.bufr jaso_214.bufr
meta_140.bufr mhen_55.bufr mhsa_55.bufr mhsb_55.bufr mhse_55.bufr modi_87.bufr modw_87.bufr monw_87.bufr
ocea_131.bufr ocea_21.bufr s4kn_165.bufr ship_11.bufr ship_12.bufr ship_13.bufr ship_14.bufr
ship_19.bufr ship_9.bufr smin_49.bufr smis_49.bufr smiu_49.bufr smos_203.bufr sn4k_165.bufr soil_7.bufr
ssbt_127.bufr stuk_7.bufr syno_1.bufr syno_2.bufr syno_3.bufr syno_4.bufr syno_multi.bufr tmr7_129.bufr
tros_31.bufr wavb_134.bufr
"

for f in $files
do
  echo "Test: packing " >> $fLog
  echo "file: $f" >> $fLog
  ${tools_dir}bufr_filter -o ${f}.out $fRules $f 2>> $fLog 1>> $fLog

  ${tools_dir}bufr_compare ${f}.out $f

  rm -f ${f}.out
done


rm -f $fRules

#-----------------------------------------------------------
# Test:  get string
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "[shipOrMobileLandStationIdentifier]";
EOF

f="ship_11.bufr"
echo "Test: get string" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
WYM9567
EOF

diff ${f}.ref ${f}.log 

rm -f ${f}.ref ${f}.log

#-----------------------------------------------------------
# Test:  get string array and stringValues
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "[stringValues!1]";
print "====";
print "[stationOrSiteName!1]";
EOF

f="synop_multi_subset.bufr"
echo "Test: get string array and stringValues" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
TROMSO-HOLT          
PASVIK               
KVITHAMAR            
FROSTA               
FURUNESET            
LOKEN I VOLBU        
APELSVOLL            
KISE                 
FAVANG               
SAERHEIM             
LANDVIK              
SANDE-GALLEBERG     

====
TROMSO-HOLT          
PASVIK               
KVITHAMAR            
FROSTA               
FURUNESET            
LOKEN I VOLBU        
APELSVOLL            
KISE                 
FAVANG               
SAERHEIM             
LANDVIK              
SANDE-GALLEBERG     

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
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_filter should have failed if key not found" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}bufr_filter -f $fRules $f 2>>$fLog 1>>$fLog


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
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_filter should have failed if key value is not allowed" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}bufr_filter -f $fRules $f 2>>$fLog 1>>$fLog


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
result=`${tools_dir}bufr_filter  $fRules $f`
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
#${tools_dir}bufr_filter -o $fBufrTmp $fRules $f >> $fLog

#Check if the resulting bufr message is the right one
cat > $fRules <<EOF
set unpack=1;
print "[typicalDate] [year] [airTemperatureAt2M%.1f]";
EOF

#[ `${tools_dir}bufr_filter $fRules $fBufrTmp` = "20010511 2001 234.5" ]

#Clean up
rm -f ${dSplit}/*
rm -f $fLog $fRules 
rm -f $fBufrTmp | true

#-----------------------------------------------------------
# Test: set unexpandedDescriptors no create new data
#-----------------------------------------------------------
f="syno_1.bufr"
testScript="set_unexpandedDescriptors_test.sh"
echo "Test: set unexpandedDescriptors no create new data" >> $fLog
echo "file: $f" >> $fLog

cat >$fRules <<EOF
set unpack=1;
set createNewData=0;
set unexpandedDescriptors={307005,13023,13013,222000,101049,31031,1031,1032,101049,33007};
write;

EOF

${tools_dir}bufr_filter -o ${f}.out $fRules $f 2>> $fLog 1>> $fLog
${tools_dir}bufr_compare ${f}.out $f 2>> $fLog 1>> $fLog

rm -f  ${f}.out 

#-----------------------------------------------------------
# Test: set keys in data section
#-----------------------------------------------------------
f="syno_1.bufr"
fout="311001.bufr"
echo "Test: set keys in data section" >> $fLog
echo "file: $f" >> $fLog

cat >$fRules <<EOF
set masterTablesVersionNumber=20;
set localTablesVersionNumber=0;
set compressedData=1;
set numberOfSubsets=10;
set unexpandedDescriptors={311001};

set windSpeed={1,2,3,4,5,6,7,8,9,10};
set windDirection={3,4,5,6,7,8,9,10,11};
set aircraftFlightNumber="1234";

set pack=1;
write;

EOF

${tools_dir}bufr_filter -o ${fout} $fRules $f 2>> $fLog 1>> $fLog
${tools_dir}bufr_compare $fout ${fout}.ref 2>> $fLog 1>> $fLog

#-----------------------------------------------------------
# ECC-147
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
set relativeHumidity=27;
set horizontalVisibility=1500;
set pack=1;
write;
EOF

f="syno_1.bufr"
${tools_dir}bufr_filter -o ${f}.out $fRules $f
# This part of the test is meant to fail
set +e
${tools_dir}bufr_compare ${f}.out $f
status=$?
set -e
if [ $status -eq 0 ]; then
  # compare should have failed and returned a non-zero exit code
  exit 1
fi
# Now blacklist the failing keys and it should pass
${tools_dir}bufr_compare -b relativeHumidity,horizontalVisibility ${f}.out $f

rm -f ${f}.out 

rm -f $fRules ${fout} $fLog

#!/bin/sh
# Copyright 2005-2016 ECMWF.
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
brightnessTemperature->percentConfidence={2147483647,0,2147483647,55,50,83,83,1,
0,17,0,2147483647,2147483647,2147483647,2147483647,2147483647,
48,16,29,14,11,15,2147483647,39,
21,0,0,2147483647,9,4,20,86,
71,96,96,68,0,0,0,96,
0,95,66,57,96,88,95,0,
97,76,94,90,92,94,89,77,
6,48,10,4,10,23,59,45,
94,3,9,58,64,2147483647,61,84,
85,27,6,47,39,65,0,0,
5,48,3,45,71,26,5,71,
64,67,57,88,90,0,96,98,
99,89,86,97,71,50,55,82,
95,83,51,97,97,86,59,87,
77,46,1,0,2147483647,0,0,0,
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

rm -f ${f}.log
${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log

REFERENCE_FILE="../../tests/asca_139.t1.ref"
diff $REFERENCE_FILE ${f}.log

rm -f ${f}.log

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

files=" 207003.bufr aaen_55.bufr aben_55.bufr ahws_139.bufr airc_142.bufr airc_144.bufr airs_57.bufr alws_139.bufr
amda_144.bufr amsa_55.bufr amsb_55.bufr amse_55.bufr amsu_55.bufr amv2_87.bufr amv3_87.bufr asbh_139.bufr asbl_139.bufr
asca_139.bufr asch_139.bufr ascs_139.bufr aseh_139.bufr asel_139.bufr ashs_139.bufr atap_55.bufr ateu_155.bufr atms_201.bufr
atov_55.bufr avhm_87.bufr avhn_87.bufr avhr_58.bufr b002_95.bufr b002_96.bufr b003_56.bufr b004_145.bufr b005_87.bufr
b005_89.bufr b006_96.bufr b007_31.bufr bssh_170.bufr bssh_176.bufr bssh_178.bufr bssh_180.bufr btem_109.bufr buoy_27.bufr
cmwi_87.bufr cmwn_87.bufr cnow_28.bufr cori_156.bufr crit_202.bufr csrh_189.bufr emsg_189.bufr emsg_87.bufr euwv_87.bufr
fy3a_154.bufr fy3b_154.bufr g2nd_208.bufr g2to_206.bufr go15_87.bufr goee_87.bufr goes_87.bufr goga_89.bufr gosat.bufr
grst_26.bufr gsd1_208.bufr gsd2_208.bufr gsd3_208.bufr gst4_26.bufr hirb_55.bufr hirs_55.bufr ias1_240.bufr iasi_241.bufr
ifco_208.bufr ikco_217.bufr itrg_208.bufr itwt_233.bufr j2eo_216.bufr j2nb_216.bufr jaso_214.bufr kond_209.bufr maer_207.bufr
meta_140.bufr mhen_55.bufr mhsa_55.bufr mhsb_55.bufr mhse_55.bufr mloz_206.bufr modi_87.bufr modw_87.bufr monw_87.bufr
new.bufr nomi_206.bufr nos1_208.bufr nos2_208.bufr nos3_208.bufr nos4_208.bufr nos5_208.bufr nos6_208.bufr nos7_208.bufr
nos8_208.bufr ocea_131.bufr ocea_132.bufr ocea_133.bufr ocea_21.bufr pgps_110.bufr pilo_91.bufr rada_250.bufr rado_250.bufr
s4kn_165.bufr sb19_206.bufr sbu8_206.bufr ship_11.bufr ship_12.bufr ship_13.bufr ship_14.bufr ship_19.bufr ship_9.bufr smin_49.bufr
smis_49.bufr smiu_49.bufr smos_203.bufr sn4k_165.bufr soil_7.bufr ssbt_127.bufr stuk_7.bufr syno_1.bufr syno_2.bufr syno_3.bufr
syno_4.bufr syno_multi.bufr synop_multi_subset.bufr temp_101.bufr temp_102.bufr temp_106.bufr tmr7_129.bufr tropical_cyclone.bufr
tros_31.bufr uegabe.bufr wavb_134.bufr"


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
# Test: set BUFRTemplate
#-----------------------------------------------------------
f="temp_101.bufr"
fOut="syno.bufr.out"
fRef="syno.bufr.out.ref"
echo "Test: set BUFRTemplate" >> $fLog
echo "file: $f" >> $fLog

cat >$fRules <<EOF
set BufrTemplate="synopLand";
write;
EOF

${tools_dir}bufr_filter -o $fOut $fRules $f 2>> $fLog 1>> $fLog
${tools_dir}bufr_compare $fOut $fRef 2>> $fLog 1>> $fLog

rm -f $fOut 

fOut="airep.bufr.out"
fRef="airep.bufr.out.ref"

cat >$fRules <<EOF
set BufrTemplate="aircraftReportWithSecondsAndPressure";
write;
EOF

${tools_dir}bufr_filter -o $fOut $fRules $f 2>> $fLog 1>> $fLog
${tools_dir}bufr_compare $fOut $fRef 2>> $fLog 1>> $fLog

rm -f $fOut 

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
set windDirection={5,3,4,5,6,7,8,9,10,11};
set aircraftFlightNumber={"ABCD","dfasd","qwerqwe","3241234","ywer","ABCD","dfasd","qwerqwe","3241234","erwe"};

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
#-----------------------------------------------------------
# Test:  access subsets by condition 
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "stationId=[/subsetNumber=6/blockNumber!%.2d][/subsetNumber=6/stationNumber!%.3d]";
print "latitude=[/subsetNumber=6/latitude]";
print "longitude=[/subsetNumber=6/longitude]";
print "airTemperature=[/subsetNumber=6/airTemperature]";
print "--------";
print "stationId=[/subsetNumber=9/blockNumber!%.2d][/subsetNumber=9/stationNumber!%.3d]";
print "latitude=[/subsetNumber=9/latitude]";
print "longitude=[/subsetNumber=9/longitude]";
print "airTemperature=[/subsetNumber=9/airTemperature]";
EOF

f="synop_multi_subset.bufr"
echo "Test: access subsets by condition" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
stationId=01371
latitude=61.122
longitude=9.063
airTemperature=265.35
--------
stationId=01387
latitude=61.455
longitude=10.1857
airTemperature=267.55
EOF

diff ${f}.ref ${f}.log 

rm -f ${f}.ref ${f}.log

#-----------------------------------------------------------
# Test:  access subsets and attribute by condition 
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "/subsetNumber=1/airTemperature->percentConfidence=[/subsetNumber=1/airTemperature->percentConfidence] [/subsetNumber=1/airTemperature->percentConfidence->units]";
EOF

f="amda_144.bufr"
echo "Test: access subsets and attribute by condition" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
/subsetNumber=1/airTemperature->percentConfidence=70 %
EOF

diff ${f}.ref ${f}.log 

rm -f ${f}.ref ${f}.log
rm -f $fLog $fRules
#-----------------------------------------------------------
# Test:  set key by rank                           
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
set #4#airTemperature = 300.1;
print "#4#airTemperature=[#4#airTemperature]";
EOF

f="temp_101.bufr"
echo "Test: set key by rank" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
cat > ${f}.ref <<EOF
#4#airTemperature=300.1
EOF

diff ${f}.ref ${f}.log 

rm -f ${f}.ref ${f}.log
rm -f $fLog $fRules
#-----------------------------------------------------------
# Test:  initialise with given values of delayed replications
#-----------------------------------------------------------
cat > $fRules <<EOF
set localTablesVersionNumber=0;
set masterTablesVersionNumber=23;

set inputDelayedDescriptorReplicationFactor = {2,3};
print "inputDelayedDescriptorReplicationFactor=[inputDelayedDescriptorReplicationFactor]";
set inputExtendedDelayedDescriptorReplicationFactor = {3,4};
print "inputExtendedDelayedDescriptorReplicationFactor=[inputExtendedDelayedDescriptorReplicationFactor]";

set numberOfSubsets=2;

set unexpandedDescriptors={309052};

print "/subsetNumber=1/delayedDescriptorReplicationFactor=[/subsetNumber=1/delayedDescriptorReplicationFactor]";
print "/subsetNumber=1/extendedDelayedDescriptorReplicationFactor=[/subsetNumber=1/extendedDelayedDescriptorReplicationFactor]";

print "/subsetNumber=2/delayedDescriptorReplicationFactor=[/subsetNumber=2/delayedDescriptorReplicationFactor]";
print "/subsetNumber=2/extendedDelayedDescriptorReplicationFactor=[/subsetNumber=2/extendedDelayedDescriptorReplicationFactor]";

set pressure={102400,50000,40000,30000,20000,15000,102400,50000,40000,30000,20000,15000};
set pack=1;

print "pressure={[pressure!12',']}";

write;
EOF

f="syno_1.bufr"
fOut="new_replication.bufr"
echo "Test: initialise with given values of delayed replications" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter -o ${fOut} $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_filter -o ${fOut} $fRules $f > ${fOut}.log

cat > ${fOut}.log.ref <<EOF
inputDelayedDescriptorReplicationFactor=2 3
inputExtendedDelayedDescriptorReplicationFactor=3 4
/subsetNumber=1/delayedDescriptorReplicationFactor=2
/subsetNumber=1/extendedDelayedDescriptorReplicationFactor=3
/subsetNumber=2/delayedDescriptorReplicationFactor=3
/subsetNumber=2/extendedDelayedDescriptorReplicationFactor=4
pressure={102400,50000,40000,30000,20000,15000,102400,50000,40000,30000,20000,15000
}
EOF

diff ${fOut}.log.ref ${fOut}.log 
${tools_dir}bufr_compare ${fOut} ${fOut}.ref

rm -f ${fOut}.log
rm -f $fLog $fRules ${fOut}
#-----------------------------------------------------------
# Test:  add section 2
#-----------------------------------------------------------
cat > $fRules <<EOF
set bufrHeaderCentre=98;
set section2Present=1;

write;
EOF

f="vos308014_v3_26.bufr"
fOut="vos308014_v3_26_sec_2.bufr"
echo "Test: initialise with given values of delayed replications" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter -o ${fOut} $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}bufr_ls ${fOut} > ${fOut}.log

cat > ${fOut}.log.ref <<EOF
vos308014_v3_26_sec_2.bufr
centre                     masterTablesVersionNumber  localTablesVersionNumber   rdbType                    rdbSubtype                 rdbtimeYear                rdbtimeMonth               typicalDate                typicalTime                numberOfSubsets            numberOfObservations       satelliteID                
ecmf                       26                         0                          0                          0                          0                          0                          00000000                   000000                     40                         0                          0                         
1 of 1 messages in vos308014_v3_26_sec_2.bufr

1 of 1 total messages in 1 files
EOF

diff ${fOut}.log.ref ${fOut}.log 

rm -f ${fOut}.log
rm -f $fLog $fRules ${fOut}

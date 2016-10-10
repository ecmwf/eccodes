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
   ${tools_dir}codes_bufr_filter $fRules $f >> $fLog
   ${tools_dir}bufr_filter       $fRules $f >> $fLog  # See ECC-205
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
${tools_dir}codes_bufr_filter $fRules $f >> $fLog

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
${tools_dir}codes_bufr_filter $fRules $f >> $fLog

#Check if the resulting bufr message is the right one
cat > $fRules <<EOF
set unpack=1;
transient statid=1000*blockNumber+stationNumber;
print "[statid]";
EOF

[ `${tools_dir}codes_bufr_filter $fRules $fBufrTmp` = "1003" ] 

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
${tools_dir}codes_bufr_filter $fRules $f >> $fLog

#Check if the resulting files exist
for statid  in 1 3 7 ; do
    [ -s ${dSplit}/split_98_13_1_1_${statid}.bufr ]
done

#-----------------------------------------------------------
# Test:  print attributes
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "pressure=[nonCoordinatePressure] [nonCoordinatePressure->units]";
print "pressure->code=[nonCoordinatePressure->code!06d]";
print "pressure->scale=[nonCoordinatePressure->scale]";
print "pressure->reference=[nonCoordinatePressure->reference]";
print "pressure->width=[nonCoordinatePressure->width]";
print "pressure->percentConfidence=[nonCoordinatePressure->percentConfidence] [nonCoordinatePressure->percentConfidence->units]";
print "pressure->percentConfidence->code=[nonCoordinatePressure->percentConfidence->code!06d]";
print "pressure->percentConfidence->scale=[nonCoordinatePressure->percentConfidence->scale]";
print "pressure->percentConfidence->reference=[nonCoordinatePressure->percentConfidence->reference]";
print "pressure->percentConfidence->width=[nonCoordinatePressure->percentConfidence->width]";
EOF

f="syno_1.bufr"
echo "Test: attributes" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

rm -f ${f}.log
${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log

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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
#fRulesReady="set_unexpandedDescriptors.filter"
#f="syno_1.bufr"
#testScript="set_unexpandedDescriptors_test.sh"
#testScript1="set_unexpandedDescriptors_test_1.sh"
#echo "Test: set unexpandedDescriptors big test" >> $fLog
#echo "file: $f" >> $fLog

#cat >$testScript <<EOF
#set -e
#
#EOF
#echo "set -x" > $testScript1
#chmod +x $testScript1

#${tools_dir}codes_bufr_filter $fRulesReady $f 2>> $fLog 1>> $testScript
#sed -e "s:diff:${tools_dir}bufr_compare:" < $testScript >> $testScript1
#./$testScript1
#rm -f new_*bufr 
#rm -f $testScript $testScript1

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
s4kn_165.bufr sb19_206.bufr sbu8_206.bufr ship_13.bufr ship_19.bufr ship_9.bufr smin_49.bufr
smis_49.bufr smiu_49.bufr smos_203.bufr sn4k_165.bufr soil_7.bufr ssbt_127.bufr stuk_7.bufr syno_1.bufr syno_3.bufr
syno_4.bufr syno_multi.bufr synop_multi_subset.bufr temp_101.bufr temp_102.bufr temp_106.bufr tmr7_129.bufr tropical_cyclone.bufr
tros_31.bufr wavb_134.bufr"


for f in $files
do
  echo "Test: packing " >> $fLog
  echo "file: $f" >> $fLog
  ${tools_dir}codes_bufr_filter -o ${f}.out $fRules $f 2>> $fLog 1>> $fLog

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

f="ship_9.bufr"
echo "Test: get string" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_filter should have failed if key not found" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}codes_bufr_filter -f $fRules $f 2>>$fLog 1>>$fLog


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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog
if [ $? -eq 0 ]; then
   echo "bufr_filter should have failed if key value is not allowed" >&2
   exit 1
fi
set -e

# Now repeat with -f option (do not exit on error)
${tools_dir}codes_bufr_filter -f $fRules $f 2>>$fLog 1>>$fLog


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
result=`${tools_dir}codes_bufr_filter  $fRules $f`
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
#${tools_dir}codes_bufr_filter -o $fBufrTmp $fRules $f >> $fLog

#Check if the resulting bufr message is the right one
cat > $fRules <<EOF
set unpack=1;
print "[typicalDate] [year] [airTemperatureAt2M%.1f]";
EOF

#[ `${tools_dir}codes_bufr_filter $fRules $fBufrTmp` = "20010511 2001 234.5" ]

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

${tools_dir}codes_bufr_filter -o ${f}.out $fRules $f 2>> $fLog 1>> $fLog
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

${tools_dir}codes_bufr_filter -o $fOut $fRules $f 2>> $fLog 1>> $fLog
${tools_dir}bufr_compare $fOut $fRef 2>> $fLog 1>> $fLog

rm -f $fOut 

fOut="airep.bufr.out"
fRef="airep.bufr.out.ref"

cat >$fRules <<EOF
set BufrTemplate="aircraftReportWithSecondsAndPressure";
write;
EOF

${tools_dir}codes_bufr_filter -o $fOut $fRules $f 2>> $fLog 1>> $fLog
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

${tools_dir}codes_bufr_filter -o ${fout} $fRules $f 2>> $fLog 1>> $fLog
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
${tools_dir}codes_bufr_filter -o ${f}.out $fRules $f
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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
${tools_dir}codes_bufr_filter $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter $fRules $f 2>> ${f}.log 1>> ${f}.log
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
${tools_dir}codes_bufr_filter -o ${fOut} $fRules $f 2>> $fLog 1>> $fLog

${tools_dir}codes_bufr_filter -o ${fOut} $fRules $f > ${fOut}.log

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
if [ -f "$f" ]; then
  fOut="vos308014_v3_26_sec_2.bufr"
  echo "Test: initialise with given values of delayed replications" >> $fLog
  echo "file: $f" >> $fLog
  ${tools_dir}codes_bufr_filter -o ${fOut} $fRules $f 2>> $fLog 1>> $fLog

  ${tools_dir}bufr_ls ${fOut} > ${fOut}.log

  cat > ${fOut}.log.ref <<EOF
vos308014_v3_26_sec_2.bufr
centre                     masterTablesVersionNumber  localTablesVersionNumber   typicalDate                typicalTime                rdbType                    rdbSubtype                 rdbtimeYear                rdbtimeMonth               numberOfSubsets            localNumberOfObservations  satelliteID                
ecmf                       26                         0                          20150107                   142500                     0                          0                          0                          0                          40                         0                          0                         
1 of 1 messages in vos308014_v3_26_sec_2.bufr

1 of 1 total messages in 1 files
EOF

  diff ${fOut}.log.ref ${fOut}.log
fi

rm -f ${fOut}.log
rm -f $fLog $fRules ${fOut}

#-----------------------------------------------------------
# Test:  extract subsets uncompressed data
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;

set extractSubset=4;
set doExtractSubsets=1;
write;

set extractSubset=2;
set doExtractSubsets=1;
write;

set extractSubsetIntervalStart=5;
set extractSubsetIntervalEnd=8;
set doExtractSubsets=1;
write;

set extractSubsetList={1,3};
set doExtractSubsets=1;
write;
EOF

f="synop_multi_subset.bufr"
fOut="extract.bufr"

echo "Test: extract subsets uncompressed data" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}codes_bufr_filter -o ${fOut} $fRules $f 2>> $fLog 1>> $fLog

cat > ${fRules} <<EOF
set unpack=1;
print "stationNumber=[stationNumber!13]";
EOF

${tools_dir}codes_bufr_filter $fRules $f $fOut > ${fOut}.log

cat > ${fOut}.log.ref <<EOF
stationNumber=27 84 270 272 308 371 381 382 387 413 464 485
stationNumber=272
stationNumber=84
stationNumber=308 371 381 382 84
stationNumber=308 371 381 382 84 27 270
EOF

diff ${fOut}.log.ref ${fOut}.log 

rm -f ${fOut}.log ${fOut}.log.ref
rm -f $fLog $fRules ${fOut}

#-----------------------------------------------------------
# Test:  associatedField
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;

print "/height=918/windDirection->associatedField=[/height=918/windDirection->associatedField]";
print "/height=918/windDirection->associatedField->associatedFieldSignificance=[/height=918/windDirection->associatedField->associatedFieldSignificance]";
EOF

f="profiler_european.bufr"

echo "Test: associatedField" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}codes_bufr_filter $fRules $f  > ${f}.log

cat > ${f}.log.ref <<EOF
/height=918/windDirection->associatedField=1
/height=918/windDirection->associatedField->associatedFieldSignificance=21
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref
rm -f $fLog $fRules 

#-----------------------------------------------------------
# Test:  extract subsets compressed data
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;

set extractSubset=10;
set doExtractSubsets=1;
write;

set extractSubsetIntervalStart=3;
set extractSubsetIntervalEnd=8;
set doExtractSubsets=1;
write;
EOF

f="g2nd_208.bufr"
fOut="g2nd_208.bufr.out"

echo "Test: extract subsets compressed data" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter -o $fOut $fRules $f 2>> $fLog 1>> $fLog

cat > ${fRules} <<EOF
set unpack=1;
print "=== message number [count]";
print "numberOfSubsets=[numberOfSubsets]";
print "solarElevation=[solarElevation!10]";
print "fieldOfViewNumber=[fieldOfViewNumber!10]";
print "orbitNumber=[orbitNumber!10]";
print "casRegistryNumber=[casRegistryNumber!10]";
EOF

${tools_dir}bufr_filter $fRules $fOut  > ${f}.log

cat > ${f}.log.ref <<EOF
=== message number 1
numberOfSubsets=1
solarElevation=33.2
fieldOfViewNumber=1
orbitNumber=2147483647
casRegistryNumber=10102-44-0
=== message number 2
numberOfSubsets=7
solarElevation=29.71 29.23 37.21 36.78 36.34 35.46 33.2
fieldOfViewNumber=2 2 0 0 0 1 1
orbitNumber=2147483647
casRegistryNumber=10102-44-0
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref
rm -f $fLog $fOut $fRules 

#-----------------------------------------------------------
# Test:  firstOrderStatistics
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "decimalScaleOfFollowingSignificands->firstOrderStatisticalValue=[/pressure=101325/decimalScaleOfFollowingSignificands->firstOrderStatisticalValue!10]";
print "decimalScaleOfFollowingSignificands->firstOrderStatisticalValue->firstOrderStatisticalValue=[/pressure=101325/decimalScaleOfFollowingSignificands->firstOrderStatisticalValue->firstOrderStatisticalValue!10]";
print "/firstOrderStatistics=9/decimalScaleOfFollowingSignificands=[/firstOrderStatistics=9/decimalScaleOfFollowingSignificands!10]";
print "/firstOrderStatistics=14/decimalScaleOfFollowingSignificands=[/firstOrderStatistics=14/decimalScaleOfFollowingSignificands!10]";
EOF

f="gosat.bufr"

echo "Test: firstOrderStatistics" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter $fRules $f  > ${f}.log

cat > ${f}.log.ref <<EOF
decimalScaleOfFollowingSignificands->firstOrderStatisticalValue=-15 -15 -15 -15 -15 -15 -15 -15 -15 -15 
-15 -15 -14 -14 -14 -15 -15 -15 -15 -15 
-15 -15 -15 -15 -15 -15 -15 -15 -15 -15 
-15 -15 -15 -15 -15 -15 -15
decimalScaleOfFollowingSignificands->firstOrderStatisticalValue->firstOrderStatisticalValue=0
/firstOrderStatistics=9/decimalScaleOfFollowingSignificands=-15 -15 -15 -15 -15 -15 -15 -15 -15 -15 
-15 -15 -14 -14 -14 -15 -15 -15 -15 -15 
-15 -15 -15 -15 -15 -15 -15 -15 -15 -15 
-15 -15 -15 -15 -15 -15 -15
/firstOrderStatistics=14/decimalScaleOfFollowingSignificands=0
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref
rm -f $fLog $fRules 

#-----------------------------------------------------------
# Test:  delayed replication compressed data
#-----------------------------------------------------------
cat > $fRules <<EOF
set localTablesVersionNumber=1;
set masterTablesVersionNumber=13;


set inputDelayedDescriptorReplicationFactor = {5};
set compressedData=1;
set numberOfSubsets=2;

set unexpandedDescriptors={312061};

set #1#windSpeedAt10M={10,20};
set #3#windSpeedAt10M={30,40};

set pack=1;

write;
EOF

f="asel_139.bufr"
fOut="asel_139.bufr.out"

echo "Test: delayed replication compressed data" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter -o $fOut $fRules $f 2>> $fLog 1>> $fLog

cat > ${fRules} <<EOF
set unpack=1;

print "delayedDescriptorReplicationFactor=[delayedDescriptorReplicationFactor]";
print "#1#windSpeedAt10M=[#1#windSpeedAt10M]";
print "#3#windSpeedAt10M=[#3#windSpeedAt10M]";
print "#5#windSpeedAt10M=[#5#windSpeedAt10M]";
EOF

${tools_dir}bufr_filter $fRules $fOut  > ${f}.log

cat > ${f}.log.ref <<EOF
delayedDescriptorReplicationFactor=5
#1#windSpeedAt10M=10 20
#3#windSpeedAt10M=30 40
#5#windSpeedAt10M=-1e+100
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref
rm -f $fLog $fOut $fRules 

#-----------------------------------------------------------
# Test:  create new BUFR with bitmap
#-----------------------------------------------------------
cat > $fRules <<EOF
set compressedData=1;
set localTablesVersionNumber=101;
set masterTablesVersionNumber=13;
set inputDelayedDescriptorReplicationFactor={4,1,4,15,2,2};
set inputDataPresentIndicator={1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
set numberOfSubsets=5;

set unexpandedDescriptors={1211, 310022, 301011, 301013, 301021, 102004, 27001, 28001, 7022, 5040,
  5043, 20010, 20016, 13040, 10001, 8043, 8044, 8023, 106000, 31001,
  207002, 10004, 207000, 8090, 15008, 8090, 8023, 33054, 10040, 114000,
  31001, 207002, 7004, 7004, 207000, 8090, 15008, 8090, 15043, 104000,
  31001, 207002, 10004, 207000, 15044, 224000, 236000, 101000, 31001, 31031,
  1033, 1032, 8023, 101000, 31001, 224255, 224000, 237000, 1033, 1032,
  8023, 101000, 31001, 224255};

write;
EOF

f="syno_1.bufr"
fOut="out.bufr"

echo "Test: create new BUFR with bitmap" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}bufr_filter -o $fOut $fRules $f 2>> $fLog 1>> $fLog

cat > ${fRules} <<EOF
set unpack=1;

print "delayedDescriptorReplicationFactor=[delayedDescriptorReplicationFactor!20]";
print "dataPresentIndicator=[dataPresentIndicator!20]";
EOF

${tools_dir}bufr_filter $fRules $fOut  > ${f}.log

cat > ${f}.log.ref <<EOF
delayedDescriptorReplicationFactor=4 1 4 15 2 2
dataPresentIndicator=1 1 0 0 1 1 1 1 1 1 1 1 1 1 1
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref
rm -f $fLog $fOut $fRules 

#-----------------------------------------------------------
# Test:  several percentConfidence for same element
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "numberOfSubsets=[numberOfSubsets]";
print "#1#windDirection";
print "[#1#windDirection!10', ']";
print "#1#windDirection->percentConfidence";
print "[#1#windDirection->percentConfidence!10', ']";
print "#1#windDirection->percentConfidence->percentConfidence";
print "[#1#windDirection->percentConfidence->percentConfidence!10', ']";
print "#1#windDirection->percentConfidence->percentConfidence->percentConfidence";
print "[#1#windDirection->percentConfidence->percentConfidence->percentConfidence!10', ']";
print "#2#windDirection";
print "[#2#windDirection!10', ']";
print "#2#windDirection->percentConfidence=[#2#windDirection->percentConfidence]";
print "#2#windDirection->percentConfidence->percentConfidence=[#2#windDirection->percentConfidence->percentConfidence]";
print "#2#windDirection->percentConfidence->percentConfidence->percentConfidence=[#2#windDirection->percentConfidence->percentConfidence->percentConfidence]";
print "#3#windDirection";
print "[#3#windDirection!10', ']";
print "#3#windDirection->percentConfidence=[#3#windDirection->percentConfidence]";
print "#3#windDirection->percentConfidence->percentConfidence=[#3#windDirection->percentConfidence->percentConfidence]";
print "#3#windDirection->percentConfidence->percentConfidence->percentConfidence=[#3#windDirection->percentConfidence->percentConfidence->percentConfidence]";
EOF

f="go15_87.bufr"

echo "Test: create new BUFR with bitmap" >> $fLog
echo "file: $f" >> $fLog

${tools_dir}bufr_filter $fRules $f  > ${f}.log

cat > ${f}.log.ref <<EOF
numberOfSubsets=128
#1#windDirection
335, 336, 341, 342, 337, 339, 343, 348, 343, 348, 
352, 351, 9, 15, 8, 8, 6, 13, 16, 29, 
18, 16, 2, 359, 359, 30, 31, 21, 22, 27, 
23, 14, 17, 7, 37, 28, 36, 17, 20, 70, 
346, 20, 69, 59, 66, 73, 59, 82, 82, 93, 
102, 136, 246, 41, 52, 57, 68, 39, 106, 127, 
149, 169, 239, 135, 166, 323, 323, 292, 263, 238, 
232, 260, 257, 239, 290, 252, 252, 239, 242, 249, 
243, 243, 294, 277, 267, 266, 253, 252, 244, 239, 
237, 243, 242, 244, 246, 272, 261, 249, 248, 248, 
242, 250, 249, 237, 237, 244, 279, 275, 254, 248, 
251, 247, 255, 245, 255, 238, 241, 233, 278, 266, 
268, 253, 262, 259, 256, 258, 242, 250
#1#windDirection->percentConfidence
99, 92, 100, 93, 99, 90, 87, 89, 97, 87, 
91, 94, 90, 97, 94, 93, 99, 99, 98, 90, 
93, 83, 91, 97, 98, 93, 87, 89, 95, 97, 
93, 99, 93, 87, 85, 96, 75, 91, 76, 99, 
57, 58, 99, 82, 100, 59, 70, 80, 84, 80, 
85, 62, 51, 92, 82, 54, 67, 63, 67, 81, 
86, 77, 70, 79, 73, 68, 60, 81, 63, 91, 
85, 67, 78, 78, 72, 85, 91, 99, 95, 93, 
56, 83, 90, 74, 98, 97, 93, 99, 60, 98, 
96, 75, 77, 99, 95, 70, 91, 57, 81, 94, 
87, 66, 55, 90, 99, 93, 59, 72, 58, 66, 
75, 80, 94, 97, 77, 98, 99, 59, 95, 78, 
63, 97, 92, 96, 81, 86, 91, 66
#1#windDirection->percentConfidence->percentConfidence
53, 50, 57, 52, 69, 63, 62, 58, 73, 65, 
65, 62, 73, 70, 76, 75, 58, 60, 64, 58, 
76, 70, 54, 59, 61, 76, 54, 56, 71, 69, 
58, 62, 71, 53, 77, 59, 64, 51, 56, 66, 
57, 58, 68, 56, 70, 57, 63, 58, 66, 65, 
67, 65, 68, 72, 72, 55, 58, 56, 57, 62, 
59, 59, 78, 60, 55, 57, 65, 54, 68, 59, 
58, 54, 58, 72, 72, 67, 66, 62, 68, 65, 
66, 76, 64, 70, 70, 65, 63, 69, 54, 63, 
62, 57, 71, 72, 65, 55, 64, 54, 73, 74, 
64, 59, 67, 67, 58, 70, 53, 69, 61, 55, 
62, 64, 59, 63, 72, 59, 62, 62, 58, 51, 
53, 63, 62, 58, 67, 68, 60, 59
#1#windDirection->percentConfidence->percentConfidence->percentConfidence
96, 90, 96, 89, 98, 91, 88, 87, 96, 86, 
89, 95, 91, 96, 93, 92, 95, 97, 96, 88, 
92, 84, 91, 97, 98, 90, 84, 88, 94, 96, 
90, 98, 93, 84, 86, 92, 77, 91, 80, 92, 
51, 54, 99, 78, 98, 62, 69, 77, 86, 80, 
80, 55, 54, 87, 83, 61, 56, 52, 59, 72, 
73, 65, 75, 71, 62, 60, 58, 69, 68, 90, 
85, 62, 73, 78, 73, 86, 90, 97, 95, 85, 
60, 86, 91, 76, 94, 95, 92, 98, 60, 94, 
94, 71, 81, 99, 95, 69, 90, 58, 84, 94, 
86, 66, 60, 90, 99, 92, 59, 71, 60, 67, 
77, 84, 94, 96, 78, 97, 88, 65, 90, 72, 
61, 96, 92, 95, 80, 86, 89, 64
#2#windDirection
326, 326, 333, 338, 337, 339, 338, 340, 341, 343, 
347, 352, 10, 13, 7, 7, 1, 16, 17, 22, 
16, 14, 9, 2, 4, 33, 25, 29, 26, 32, 
21, 20, 22, 11, 41, 34, 33, 31, 31, 68, 
33, 55, 64, 53, 58, 76, 51, 68, 84, 92, 
97, 101, 249, 38, 47, 51, 65, 68, 111, 145, 
114, 129, 242, 160, 178, 357, 346, 325, 276, 228, 
235, 242, 242, 243, 301, 253, 248, 243, 244, 243, 
243, 243, 295, 285, 272, 268, 257, 253, 245, 245, 
243, 245, 243, 241, 242, 282, 267, 259, 251, 250, 
247, 245, 246, 240, 240, 241, 280, 277, 264, 260, 
257, 252, 250, 249, 251, 240, 240, 238, 278, 263, 
262, 258, 262, 260, 255, 253, 245, 247
#2#windDirection->percentConfidence=0
#2#windDirection->percentConfidence->percentConfidence=0
#2#windDirection->percentConfidence->percentConfidence->percentConfidence=0
#3#windDirection
335, 336, 341, 342, 337, 339, 343, 348, 343, 348, 
352, 351, 9, 15, 8, 8, 6, 13, 16, 29, 
18, 16, 2, 359, 359, 30, 31, 21, 22, 27, 
23, 14, 17, 7, 37, 28, 36, 17, 20, 70, 
346, 20, 69, 59, 66, 73, 59, 82, 82, 93, 
102, 136, 246, 41, 52, 57, 68, 39, 106, 127, 
149, 169, 239, 135, 166, 323, 323, 292, 263, 238, 
232, 260, 257, 239, 290, 252, 252, 239, 242, 249, 
243, 243, 294, 277, 267, 266, 253, 252, 244, 239, 
237, 243, 242, 244, 246, 272, 261, 249, 248, 248, 
242, 250, 249, 237, 237, 244, 279, 275, 254, 248, 
251, 247, 255, 245, 255, 238, 241, 233, 278, 266, 
268, 253, 262, 259, 256, 258, 242, 250
#3#windDirection->percentConfidence=0
#3#windDirection->percentConfidence->percentConfidence=0
#3#windDirection->percentConfidence->percentConfidence->percentConfidence=0
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref

#-----------------------------------------------------------
# Test: Data with two bias correction wrong bitmap
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "[nonCoordinatePressure->percentConfidence]";
print "[nonCoordinatePressure->differenceStatisticalValue]";
print "[nonCoordinatePressure->differenceStatisticalValue->differenceStatisticalValue]";
EOF

f="metar_with_2_bias.bufr"

echo "Test: Data with two bias correction wrong bitmap" >> $fLog
echo "file: $f" >> $fLog

${tools_dir}bufr_filter $fRules $f  > ${f}.log

cat > ${f}.log.ref <<EOF
70
-1e+100
-100
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref

#-----------------------------------------------------------
# Test: Data with substituted value
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;
print "[#1#nonCoordinateGeopotential]";
print "[#1#nonCoordinateGeopotential->percentConfidence]";
print "[#1#nonCoordinateGeopotential->substitutedValue]";
EOF

f="temp-land-with-substituted-values.bufr"

echo "Test: Data with substituted value" >> $fLog
echo "file: $f" >> $fLog

${tools_dir}bufr_filter $fRules $f  > ${f}.log

cat > ${f}.log.ref <<EOF
110
70
100
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref $fLog $fRules

#-----------------------------------------------------------
# Test: Nested delayed replication
#-----------------------------------------------------------
cat > $fRules <<EOF
set masterTablesVersionNumber=24;
set inputDelayedDescriptorReplicationFactor={2,3,5};
set unexpandedDescriptors={1001,1002, 107000, 31001,5001,5002,102000, 31001,7004, 12001,12004 };
print "[numericValues]";
EOF

f="syno_1.bufr"

echo "Test: Nested delayed replication" >> $fLog
echo "file: $f" >> $fLog

${tools_dir}bufr_filter $fRules $f  > ${f}.log

cat > ${f}.log.ref <<EOF
-1e+100 -1e+100 2 -1e+100 -1e+100 3 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 5 
-1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref ${f}.out $fLog $fRules
#-----------------------------------------------------------
# Test: Simple thinning
#-----------------------------------------------------------
cat > $fRules <<EOF
set simpleThinningSkip=36;
set doSimpleThinning=1;
set pack=1;
write;
EOF

f="imssnow.bufr"

echo "Test: Simple thinning" >> $fLog
echo "file: $f" >> $fLog

${tools_dir}bufr_filter -o ${f}.out $fRules $f

cat > $fRules <<EOF
set unpack=1;
print "latitude=[latitude]";
print "longitude=[longitude]";
print "height=[height]";
EOF

${tools_dir}bufr_filter $fRules ${f}.out > ${f}.log

cat > ${f}.log.ref <<EOF
latitude=4.93301 5.17216 5.40243 5.62361 7.86075
longitude=118.16205 117.41896 116.66977 115.91467 99.56805
height=119 231 587 187 23
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref ${f}.out $fLog $fRules

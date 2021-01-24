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

# Define a common label for all the tmp files
label="bufr_extract_headers_test"
temp1="temp.${label}.1"
temp2="temp.${label}.2"

# Multi-message BUFR
# --------------------
input=${data_dir}/bufr/aeolus_wmo_26.bufr
KEYS='offset,edition,totalLength'
$EXEC ${test_dir}/bufr_extract_headers  $KEYS  $input > $temp1
${tools_dir}/bufr_get                -p $KEYS  $input > $temp2
diff $temp1 $temp2

# Test local ECMWF keys; should be "not_found" for this input
# -----------------------------------------------------------
input=${data_dir}/bufr/PraticaTemp.bufr
KEYS='localSectionPresent,rdbType,ident,isSatellite,satelliteID'
$EXEC ${test_dir}/bufr_extract_headers $KEYS $input > $temp1
${tools_dir}/bufr_get            -f -p $KEYS $input > $temp2
diff $temp1 $temp2


# Local Section keys
# --------------------
# DWD BUFR with a local section
input=${data_dir}/bufr/uegabe.bufr
r=`${test_dir}/bufr_extract_headers localSectionPresent,ecmwfLocalSectionPresent $input`
[ "$r" = "1 0" ]
# ECMWF BUFR but has no local section
input=${data_dir}/bufr/207003.bufr
r=`${test_dir}/bufr_extract_headers localSectionPresent,ecmwfLocalSectionPresent $input`
[ "$r" = "0 0" ]
# ECMWF BUFR with a local section
input=${data_dir}/bufr/aaen_55.bufr
r=`${test_dir}/bufr_extract_headers localSectionPresent,ecmwfLocalSectionPresent $input`
[ "$r" = "1 1" ]

# Key 'centre' as string
# -----------------------
r=`${test_dir}/bufr_extract_headers centre ${data_dir}/bufr/aaen_55.bufr`
[ "$r" = "ecmf" ]
r=`${test_dir}/bufr_extract_headers centre ${data_dir}/bufr/uegabe.bufr`
[ "$r" = "edzw" ]
r=`${test_dir}/bufr_extract_headers centre ${data_dir}/bufr/synop_multi_subset.bufr`
[ "$r" = "enmi" ]
r=`${test_dir}/bufr_extract_headers centre ${data_dir}/bufr/PraticaTemp.bufr`
[ "$r" = "cnmc" ]
r=`${test_dir}/bufr_extract_headers centre ${data_dir}/bufr/israel_observations_2017041010.bufr`
[ "$r" = "234" ]

# Check all centres with an abbreviation
centre_table=${ECCODES_DEFINITION_PATH}/common/c-11.table
centres=`awk 'NR > 1 {print $2}' < $centre_table`
for c in $centres; do
    ${tools_dir}/bufr_set -s centre=$c $ECCODES_SAMPLES_PATH/BUFR4.tmpl $temp1
    r=`${test_dir}/bufr_extract_headers centre $temp1`
    [ "$r" = "$c" ]
done

# Test rdbSubtype
# ---------------
fBufr3Input=$ECCODES_SAMPLES_PATH/BUFR3_local.tmpl
fBufr4Input=$ECCODES_SAMPLES_PATH/BUFR4_local.tmpl
inputs="$fBufr3Input $fBufr4Input"
for fin in $inputs; do
  ${tools_dir}/bufr_set -s oldSubtype=2,newSubtype=300 $fin $temp1
  r=`${test_dir}/bufr_extract_headers rdbSubtype,oldSubtype,newSubtype $temp1`
  [ "$r" = "2 2 300" ]

  ${tools_dir}/bufr_set -s oldSubtype=255,newSubtype=300 $fin $temp1
  res=`${test_dir}/bufr_extract_headers rdbSubtype,oldSubtype,newSubtype $temp1`
  [ "$res" = "300 255 300" ]
done


# Test all BUFR files
# ---------------------
bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`

KEYS='edition,totalLength,bufrHeaderCentre,dataCategory,masterTablesVersionNumber,typicalMonth,typicalDay,rdbType,localYear,qualityControl,numberOfSubsets,compressedData,ident'

for bf in ${bufr_files}; do
    input=${data_dir}/bufr/$bf
    $EXEC ${test_dir}/bufr_extract_headers $KEYS $input > $temp1
    ${tools_dir}/bufr_get            -f -p $KEYS $input > $temp2
    diff $temp1 $temp2
done


# BUFRs with localLatitude1, localLongitude2 etc
# ----------------------------------------------
bufr_files="
aaen_55.bufr
aben_55.bufr
aeolus_wmo_26.bufr
ahws_139.bufr
airs_57.bufr
alws_139.bufr
amsa_55.bufr
amsb_55.bufr
amse_55.bufr
amsu_55.bufr
amv2_87.bufr
amv3_87.bufr
asbh_139.bufr
asbl_139.bufr
asca_139.bufr
asch_139.bufr
ascs_139.bufr
aseh_139.bufr
asel_139.bufr
ashs_139.bufr
atap_55.bufr
ateu_155.bufr
atms_201.bufr
atov_55.bufr
avhm_87.bufr
avhn_87.bufr
avhr_58.bufr
b003_56.bufr
b005_87.bufr
b005_89.bufr
b007_31.bufr
cmwi_87.bufr
cmwn_87.bufr
cori_156.bufr
crit_202.bufr
csrh_189.bufr
emsg_189.bufr
emsg_87.bufr
euwv_87.bufr
fy3a_154.bufr
fy3b_154.bufr
g2nd_208.bufr
g2to_206.bufr
go15_87.bufr
goee_87.bufr
goes_87.bufr
goga_89.bufr
gosat.bufr
grst_26.bufr
gsd1_208.bufr
gsd2_208.bufr
gsd3_208.bufr
gst4_26.bufr
hirb_55.bufr
hirs_55.bufr
ias1_240.bufr
iasi_241.bufr
ifco_208.bufr
ikco_217.bufr
imssnow.bufr
itrg_208.bufr
itwt_233.bufr
j2eo_216.bufr
j2nb_216.bufr
jaso_214.bufr
kond_209.bufr
maer_207.bufr
mhen_55.bufr
mhsa_55.bufr
mhsb_55.bufr
mhse_55.bufr
mloz_206.bufr
modi_87.bufr
modw_87.bufr
monw_87.bufr
nomi_206.bufr
nos1_208.bufr
nos2_208.bufr
nos3_208.bufr
nos4_208.bufr
nos5_208.bufr
nos6_208.bufr
nos7_208.bufr
nos8_208.bufr
pgps_110.bufr
rada_250.bufr
rado_250.bufr
s4kn_165.bufr
sb19_206.bufr
sbu8_206.bufr
smin_49.bufr
smis_49.bufr
smiu_49.bufr
smos_203.bufr
sn4k_165.bufr
ssbt_127.bufr
tmr7_129.bufr
tropical_cyclone.bufr
tros_31.bufr
"

KEYS='localLongitude1,localLatitude1,localLongitude2,localLatitude2,localNumberOfObservations,satelliteID,restricted'
for bf in ${bufr_files}; do
    input=${data_dir}/bufr/$bf
    $EXEC ${test_dir}/bufr_extract_headers $KEYS $input > $temp1
    ${tools_dir}/bufr_get            -f -p $KEYS $input > $temp2
    diff $temp1 $temp2
done

# Test restricted
${tools_dir}/bufr_set -s restricted=1 ${data_dir}/bufr/aaen_55.bufr $temp1
r=`$EXEC ${test_dir}/bufr_extract_headers restricted $temp1`
[ "$r" = "1" ]


echo "Test with invalid inputs..."
# ---------------------------------
set +e
$EXEC ${test_dir}/bufr_extract_headers centre ${data_dir}
status=$?
set -e
[ $status -ne 0 ]

rm -f $temp1 $temp2

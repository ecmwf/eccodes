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
label="bufr_dump_filter_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp bufr file
fBufrTmp=${label}".bufr.tmp"

#Define filter rules file
fRules=${label}.filter

#-----------------------------------------------------------
# TODO: not all of our BUFR files pass this test!!
# The following do not work:
# bufr_filter fails:
#   ias1_240.bufr           -- too large, memory exhausted!
#   tropical_cyclone.bufr   -- Segmentation fault
# bufr_compare fails:
#   ocea_133.bufr   -- delayedDescriptorReplicationFactor->percentConfidence
#   pilo_91.bufr    -- ditto
#   syno_multi.bufr  -- lots of diffs
#   temp-land-with-substituted-values.bufr -- delayedDescriptorReplicationFactor->percentConfidence
#   temp_101.bufr -- #1#delayedDescriptorReplicationFactor->percentConfidence
#   temp_102.bufr -- percentConfidence missing
#   temp_106.bufr -- percentConfidence missing
#-----------------------------------------------------------
files="
207003.bufr       ashs_139.bufr  cmwn_87.bufr   hirs_55.bufr            nos1_208.bufr           ship_9.bufr
PraticaTemp.bufr  atap_55.bufr   cnow_28.bufr   iasi_241.bufr           nos2_208.bufr           smin_49.bufr
aaen_55.bufr      ateu_155.bufr  cori_156.bufr  ifco_208.bufr           nos3_208.bufr           smis_49.bufr
aben_55.bufr      atms_201.bufr  crit_202.bufr  ikco_217.bufr           nos4_208.bufr           smiu_49.bufr
ahws_139.bufr     atov_55.bufr   csrh_189.bufr  itrg_208.bufr           nos5_208.bufr           smos_203.bufr
airc_142.bufr     avhm_87.bufr   emsg_189.bufr  itwt_233.bufr           nos6_208.bufr           sn4k_165.bufr
airc_144.bufr     avhn_87.bufr   emsg_87.bufr   j2eo_216.bufr           nos7_208.bufr           soil_7.bufr
airs_57.bufr      avhr_58.bufr   euwv_87.bufr   j2nb_216.bufr           nos8_208.bufr           ssbt_127.bufr
alws_139.bufr     b002_95.bufr   fy3a_154.bufr  jaso_214.bufr           ocea_131.bufr           stuk_7.bufr
amda_144.bufr     b002_96.bufr   fy3b_154.bufr  kond_209.bufr           ocea_132.bufr           syno_1.bufr
amsa_55.bufr      b003_56.bufr   g2nd_208.bufr  maer_207.bufr           ocea_21.bufr            syno_2.bufr
amsb_55.bufr      b004_145.bufr  g2to_206.bufr  meta_140.bufr           pgps_110.bufr           syno_3.bufr
amse_55.bufr      b005_87.bufr   go15_87.bufr   metar_with_2_bias.bufr  profiler_european.bufr  syno_4.bufr
amsu_55.bufr      b005_89.bufr   goee_87.bufr   mhen_55.bufr            rada_250.bufr           synop_multi_subset.bufr
amv2_87.bufr      b006_96.bufr   goes_87.bufr   mhsa_55.bufr            rado_250.bufr           tmr7_129.bufr
amv3_87.bufr      b007_31.bufr   goga_89.bufr   mhsb_55.bufr            s4kn_165.bufr           tros_31.bufr
asbh_139.bufr     bssh_170.bufr  gosat.bufr     mhse_55.bufr            sb19_206.bufr           uegabe.bufr
asbl_139.bufr     bssh_176.bufr  grst_26.bufr   mloz_206.bufr           sbu8_206.bufr           wavb_134.bufr
asca_139.bufr     bssh_178.bufr  gsd1_208.bufr  modi_87.bufr            ship_11.bufr
asch_139.bufr     bssh_180.bufr  gsd2_208.bufr  modw_87.bufr            ship_12.bufr
ascs_139.bufr     btem_109.bufr  gsd3_208.bufr  monw_87.bufr            ship_13.bufr
aseh_139.bufr     buoy_27.bufr   gst4_26.bufr   new.bufr                ship_14.bufr
asel_139.bufr     cmwi_87.bufr   hirb_55.bufr   nomi_206.bufr           ship_19.bufr
"

exclude=""

for f in $files
do
  echo "Test: bufr_dump -Efilter " >> $fLog
  echo "file: $f" >> $fLog

  ${tools_dir}bufr_dump -Efilter $f > $fRules
  ${tools_dir}bufr_filter -o $fBufrTmp $fRules $f
  ${tools_dir}bufr_compare $fBufrTmp $f

  # TODO: Some need to be excluded from the JSON compare
  compare_dumps=1
  #for ex in $exclude; do
  #  if [ "$f" = "$ex" ]; then compare_dumps=0; fi
  #done

  if [ $compare_dumps -eq 1 ]; then
    TEMP_JSON1=${label}.$f.json
    TEMP_JSON2=${label}.$fBufrTmp.json
    ${tools_dir}bufr_dump $f        > $TEMP_JSON1
    ${tools_dir}bufr_dump $fBufrTmp > $TEMP_JSON2
    diff $TEMP_JSON1 $TEMP_JSON2
    rm -f $TEMP_JSON1 $TEMP_JSON2
  fi

  rm -f $fBufrTmp $fRules
done

rm -f $fLog $fBufrTmp $fRules

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

#Define a common label for all the tmp files
label="bufr_dump_python_test"

#Create log file
fLog=${label}".log"
rm -f $fLog

tempBufr=outfile.bufr
tempDir=${label}.dir
mkdir -p $tempDir
cd $tempDir

#bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
bufr_files="
aaen_55.bufr   atov_55.bufr   buoy_27.bufr   goes_87.bufr   itrg_208.bufr           modi_87.bufr   ocea_21.bufr   smis_49.bufr             temp_101.bufr
aben_55.bufr   avhm_87.bufr   cmwi_87.bufr   goga_89.bufr   j2eo_216.bufr           modw_87.bufr   rada_250.bufr  smiu_49.bufr             temp_106.bufr
airc_142.bufr  avhn_87.bufr   cmwn_87.bufr   gosat.bufr     j2nb_216.bufr           monw_87.bufr   rado_250.bufr  smos_203.bufr            tmr7_129.bufr
airc_144.bufr  avhr_58.bufr   cnow_28.bufr   grst_26.bufr   jaso_214.bufr           nomi_206.bufr  s4kn_165.bufr  sn4k_165.bufr            tros_31.bufr
amda_144.bufr  b002_95.bufr   cori_156.bufr  gsd1_208.bufr  kond_209.bufr           nos1_208.bufr  sb19_206.bufr  soil_7.bufr              wavb_134.bufr
amsa_55.bufr   b003_56.bufr   csrh_189.bufr  gsd2_208.bufr  maer_207.bufr           nos2_208.bufr  sbu8_206.bufr  ssbt_127.bufr
amsb_55.bufr   b004_145.bufr  emsg_189.bufr  gsd3_208.bufr  meta_140.bufr           nos3_208.bufr  ship_11.bufr   stuk_7.bufr
amse_55.bufr   b005_87.bufr   emsg_87.bufr   gst4_26.bufr   metar_with_2_bias.bufr  nos4_208.bufr  ship_12.bufr   syno_1.bufr
amsu_55.bufr   b005_89.bufr   euwv_87.bufr   hirb_55.bufr   mhen_55.bufr            nos5_208.bufr  ship_13.bufr   syno_2.bufr
amv2_87.bufr   b007_31.bufr   g2nd_208.bufr  hirs_55.bufr   mhsa_55.bufr            nos6_208.bufr  ship_14.bufr   syno_3.bufr
amv3_87.bufr   bssh_170.bufr  g2to_206.bufr  ias1_240.bufr  mhsb_55.bufr            nos7_208.bufr  ship_19.bufr   syno_4.bufr
atap_55.bufr   bssh_176.bufr  go15_87.bufr   ifco_208.bufr  mhse_55.bufr            nos8_208.bufr  ship_9.bufr    syno_multi.bufr
ateu_155.bufr  bssh_178.bufr  goee_87.bufr   ikco_217.bufr  mloz_206.bufr           ocea_131.bufr  smin_49.bufr   synop_multi_subset.bufr
"

for file in ${bufr_files}
do
  inputBufr=${data_dir}/bufr/$file
  tempSrc=$label.$file.py

  # Generate Python code from BUFR file
  ${tools_dir}bufr_dump -Epython $inputBufr > $tempSrc
  # The python code always creates an output file called outfile.bufr
  #set +e
  $PYTHON $tempSrc
  #if [ $? -eq 0 ]; then
  #   cp $inputBufr /tmp/OK
  #else
  #   cp $inputBufr /tmp/OK.NOT
  #fi

  ${tools_dir}bufr_compare $inputBufr $tempBufr

  TEMP_JSON1=${label}.$file.json
  TEMP_JSON2=${label}.$tempBufr.json
  ${tools_dir}bufr_dump $inputBufr > $TEMP_JSON1
  ${tools_dir}bufr_dump $tempBufr  > $TEMP_JSON2
  diff $TEMP_JSON1 $TEMP_JSON2 2>/dev/null
  rm -f $TEMP_JSON1 $TEMP_JSON2

  rm -f $tempSrc $tempBufr
  #set -e
done
cd $test_dir
rm -fr $tempDir

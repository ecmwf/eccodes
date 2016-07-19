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
label="bufr_dump_fortran_test"

#Create log file
fLog=${label}".log"
rm -f $fLog

#Define temp files
tempSrc=$label.temp.f90
tempExe=$label.temp.exe

# bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`

bufr_files="207003.bufr PraticaTemp.bufr aaen_55.bufr aben_55.bufr ahws_139.bufr airc_142.bufr airc_144.bufr airs_57.bufr alws_139.bufr
amda_144.bufr amsa_55.bufr amsb_55.bufr amse_55.bufr amsu_55.bufr amv2_87.bufr amv3_87.bufr asbh_139.bufr asbl_139.bufr
asca_139.bufr asch_139.bufr ascs_139.bufr aseh_139.bufr asel_139.bufr ashs_139.bufr atap_55.bufr ateu_155.bufr atms_201.bufr
atov_55.bufr avhm_87.bufr avhn_87.bufr avhr_58.bufr b002_95.bufr b002_96.bufr b003_56.bufr b004_145.bufr b005_87.bufr
b005_89.bufr b006_96.bufr b007_31.bufr bssh_170.bufr bssh_176.bufr bssh_178.bufr bssh_180.bufr btem_109.bufr buoy_27.bufr
cmwi_87.bufr cmwn_87.bufr cnow_28.bufr cori_156.bufr crit_202.bufr csrh_189.bufr emsg_189.bufr emsg_87.bufr euwv_87.bufr
fy3a_154.bufr fy3b_154.bufr g2nd_208.bufr g2to_206.bufr go15_87.bufr goee_87.bufr goes_87.bufr goga_89.bufr gosat.bufr
grst_26.bufr gsd1_208.bufr gsd2_208.bufr gsd3_208.bufr gst4_26.bufr hirb_55.bufr hirs_55.bufr iasi_241.bufr ifco_208.bufr
ikco_217.bufr itrg_208.bufr itwt_233.bufr j2eo_216.bufr j2nb_216.bufr jaso_214.bufr kond_209.bufr maer_207.bufr meta_140.bufr
mhen_55.bufr mhsa_55.bufr mhsb_55.bufr mhse_55.bufr mloz_206.bufr modi_87.bufr modw_87.bufr monw_87.bufr new.bufr nomi_206.bufr
nos1_208.bufr nos2_208.bufr nos3_208.bufr nos4_208.bufr nos5_208.bufr nos6_208.bufr nos7_208.bufr nos8_208.bufr ocea_131.bufr
ocea_132.bufr ocea_133.bufr ocea_21.bufr pgps_110.bufr pilo_91.bufr profiler_european.bufr rada_250.bufr rado_250.bufr s4kn_165.bufr
sb19_206.bufr sbu8_206.bufr ship_11.bufr ship_12.bufr ship_13.bufr ship_14.bufr ship_19.bufr ship_9.bufr smin_49.bufr smis_49.bufr
smiu_49.bufr smos_203.bufr sn4k_165.bufr soil_7.bufr ssbt_127.bufr stuk_7.bufr syno_1.bufr syno_2.bufr syno_3.bufr syno_4.bufr
synop_multi_subset.bufr temp_101.bufr temp_102.bufr temp_106.bufr tmr7_129.bufr tros_31.bufr uegabe.bufr wavb_134.bufr"


REDIRECT=/dev/null

# BUILD_CONFIG=../eccodes_ecbuild_config.h
# If FORTRAN is enabled, then the pkgconfig file is guaranteed to be here
BUILD_CONFIG=../eccodes_f90.pc

# Work out the Fortran compiler being used
#F90Compiler=`grep -w ECCODES_Fortran_COMPILER $BUILD_CONFIG | cut -d' ' -f3` | sed -e 's/"//g'
F90Compiler=`grep ^FC= $BUILD_CONFIG | cut -d'=' -f2`
LIB_ECC="../lib/libeccodes.so"
LIB_F90="../lib/libeccodes_f90.so"
MOD_DIR="../fortran/modules"
COMPILE=0

# Only works with dynamic libraries.
if [ -f $F90Compiler -a -f $LIB_ECC -a -f $LIB_F90 -a -d $MOD_DIR ]; then
  COMPILE=1
fi

for file in ${bufr_files}
do
  # Generate F90 code from BUFR file
  ${tools_dir}bufr_dump -Efortran ${data_dir}/bufr/$file > $tempSrc
  
  # Compile
  if [ $COMPILE -eq 1 ]; then
    $F90Compiler -o $tempExe $tempSrc -I $MOD_DIR  $LIB_F90  $LIB_ECC
  fi
done

rm -f $tempSrc $tempExe

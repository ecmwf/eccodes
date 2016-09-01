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
label="bufr_dump_C_test"

#Create log file
fLog=${label}".log"
rm -f $fLog

tempBufr=outfile.bufr
tempDir=${label}.dir
mkdir -p $tempDir

#bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
bufr_files="
207003.bufr    ascs_139.bufr  b006_96.bufr   goga_89.bufr   new.bufr                soil_7.bufr
aaen_55.bufr   aseh_139.bufr  btem_109.bufr  hirb_55.bufr   nomi_206.bufr           ssbt_127.bufr
aben_55.bufr   asel_139.bufr  cmwi_87.bufr   hirs_55.bufr   ocea_131.bufr           stuk_7.bufr
ahws_139.bufr  ashs_139.bufr  cmwn_87.bufr   ias1_240.bufr  ocea_21.bufr            syno_1.bufr
airs_57.bufr   atap_55.bufr   cori_156.bufr  iasi_241.bufr  pilo_91.bufr            syno_2.bufr
alws_139.bufr  ateu_155.bufr  crit_202.bufr  itwt_233.bufr  profiler_european.bufr  syno_3.bufr
amsa_55.bufr   atms_201.bufr  csrh_189.bufr  jaso_214.bufr  rada_250.bufr           syno_4.bufr
amsb_55.bufr   atov_55.bufr   emsg_189.bufr  maer_207.bufr  rado_250.bufr           syno_multi.bufr
amse_55.bufr   avhm_87.bufr   emsg_87.bufr   mhen_55.bufr   s4kn_165.bufr           temp-land-with-substituted-values.bufr
amsu_55.bufr   avhn_87.bufr   euwv_87.bufr   mhsa_55.bufr   sb19_206.bufr           temp_101.bufr
amv2_87.bufr   avhr_58.bufr   fy3a_154.bufr  mhsb_55.bufr   sbu8_206.bufr           tmr7_129.bufr
amv3_87.bufr   b002_95.bufr   fy3b_154.bufr  mhse_55.bufr   smin_49.bufr            uegabe.bufr
asbh_139.bufr  b002_96.bufr   g2to_206.bufr  mloz_206.bufr  smis_49.bufr
asbl_139.bufr  b003_56.bufr   go15_87.bufr   modi_87.bufr   smiu_49.bufr
asca_139.bufr  b005_87.bufr   goee_87.bufr   modw_87.bufr   smos_203.bufr
asch_139.bufr  b005_89.bufr   goes_87.bufr   monw_87.bufr   sn4k_165.bufr
"

# pkgconfig should be one level above the test dir
PKGCONFIG_FILE=../eccodes.pc
CACHE_FILE=../CMakeCache.txt

COMPILE_AND_RUN=0

if command -v pkg-config >/dev/null 2>&1; then
  if [ -f "$PKGCONFIG_FILE" ]; then
    # Work out the C compiler and flags from pkgconfig
    COMPILER=`pkg-config --variable=CC $PKGCONFIG_FILE`
    FLAGS_COMPILER=`pkg-config --cflags $PKGCONFIG_FILE`
    FLAGS_LINKER=`pkg-config --libs $PKGCONFIG_FILE`

    # The pkgconfig variables refer to the install directory. Change to build dir
    BUILD_DIR=`grep -w eccodes_BINARY_DIR $CACHE_FILE | cut -d'=' -f2`
    INSTALL_DIR=`grep -w CMAKE_INSTALL_PREFIX $CACHE_FILE | cut -d'=' -f2`
    FLAGS_LINKER=`echo $FLAGS_LINKER | sed -e "s:$INSTALL_DIR:$BUILD_DIR:g"`
    FLAGS_COMPILER=`echo $FLAGS_COMPILER | sed -e "s:$INSTALL_DIR:$BUILD_DIR:g"`

    # TODO: For now only support when shared libs enabled
    SHARED_LIBS=`grep -w BUILD_SHARED_LIBS $CACHE_FILE | cut -d'=' -f2`
    if [ "$SHARED_LIBS" = "ON" ]; then
      COMPILE_AND_RUN=1
    fi
  fi
fi

cd $tempDir

for file in ${bufr_files}
do
  tempSrc=$label.$file.c
  tempExe=$label.$file.exe

  # Generate C code from BUFR file
  ${tools_dir}bufr_dump -EC ${data_dir}/bufr/$file > $tempSrc

  # Compile
  if [ $COMPILE_AND_RUN -eq 1 ]; then
    # TODO: eccodes.h and the generated eccodes_version.h need to be pointed to
    #       Should be copied over to the build/include dir
    INCL_DIR1=${proj_dir}/src
    INCL_DIR2=${data_dir}/../src

    $COMPILER -o $tempExe $tempSrc -I${INCL_DIR1} -I${INCL_DIR2} $FLAGS_COMPILER $FLAGS_LINKER

    # The executable always creates a file called outfile.bufr
    # valgrind --error-exitcode=1  ./$tempExe
    ./$tempExe
    ${tools_dir}bufr_compare ${data_dir}/bufr/$file $tempBufr

    TEMP_JSON1=${label}.$file.json
    TEMP_JSON2=${label}.$tempBufr.json
    ${tools_dir}bufr_dump ${data_dir}/bufr/$file > $TEMP_JSON1
    ${tools_dir}bufr_dump $tempBufr              > $TEMP_JSON2
    diff $TEMP_JSON1 $TEMP_JSON2
    rm -f $TEMP_JSON1 $TEMP_JSON2
  fi

  rm -f $tempExe $tempSrc $tempBufr
done

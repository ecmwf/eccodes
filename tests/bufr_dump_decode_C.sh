#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

#Define a common label for all the tmp files
label="bufr_dump_decode_C_test"

if [ $HAVE_MEMFS -eq 1 ]; then
    unset ECCODES_DEFINITION_PATH
    unset ECCODES_SAMPLES_PATH
fi

#Create log file
fLog=${label}".log"
rm -f $fLog

tempDir=${label}.dir
rm -rf $tempDir
mkdir -p $tempDir

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`

# pkgconfig should be one level above the test dir
PKGCONFIG_FILE=../../eccodes.pc
CACHE_FILE=../../CMakeCache.txt

COMPILE_AND_RUN=0

cd $tempDir

if command -v pkg-config >/dev/null 2>&1; then
  if [ -f "$PKGCONFIG_FILE" ]; then
    sed -e "s#^prefix=.*#prefix=$build_dir#" < $PKGCONFIG_FILE > temp.pc
    PKGCONFIG_FILE=temp.pc
    # Work out the C compiler and flags from pkgconfig
    COMPILER=`pkg-config --variable=CC $PKGCONFIG_FILE`
    FLAGS_COMPILER=`pkg-config --cflags $PKGCONFIG_FILE`
    FLAGS_LINKER=`pkg-config --libs $PKGCONFIG_FILE`

    # TODO: For now only support when shared libs enabled
    #SHARED_LIBS=`grep -w BUILD_SHARED_LIBS $CACHE_FILE | cut -d'=' -f2`
    #if [ "$SHARED_LIBS" = "ON" ]; then
    #  COMPILE_AND_RUN=1
    #fi
  fi
fi

for file in ${bufr_files}
do
  tempSrc=$label.$file.c
  tempExe=$label.$file.exe
  filePath=${data_dir}/bufr/$file

  # Generate C code from BUFR file
  ${tools_dir}/bufr_dump -DC $filePath > $tempSrc

  # Too large for this test
  if [ "$file" = "ias1_240.bufr" ]; then
    continue
  fi

  # Compile
  if [ $COMPILE_AND_RUN -eq 1 ]; then
    # TODO: eccodes.h and the generated eccodes_version.h need to be pointed to
    #       Should be copied over to the build/include dir
    INCL_DIR1=${proj_dir}/src
    INCL_DIR2=${data_dir}/../src

    $COMPILER -o $tempExe $tempSrc -I${INCL_DIR1} -I${INCL_DIR2} $FLAGS_COMPILER $FLAGS_LINKER

    if test "x$ECCODES_TEST_WITH_VALGRIND" != "x"; then
      valgrind --error-exitcode=1 -q ./$tempExe $filePath
    else
      ./$tempExe $filePath
    fi
  fi

  rm -f $tempExe $tempSrc
done

cd $test_dir
rm -fr $tempDir

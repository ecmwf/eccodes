#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

#Define a common label for all the tmp files
label="bufr_dump_encode_C_test"

#Create log file
fLog=${label}".log"
rm -f $fLog

tempBufr=outfile.bufr
tempDir=${label}.dir
mkdir -p $tempDir

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`

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
    #CMAKE_C_FLAGS=`grep CMAKE_C_FLAGS:STRING $CACHE_FILE | cut -d'=' -f2-`
    #FLAGS_COMPILER="$FLAGS_COMPILER  -fsanitize=memory"

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
  ${tools_dir}/bufr_dump -EC ${data_dir}/bufr/$file > $tempSrc

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

    # The executable always creates a file called outfile.bufr
    if test "x$ECCODES_TEST_WITH_VALGRIND" != "x"; then
      valgrind --error-exitcode=1 -q ./$tempExe
    else
      ./$tempExe
    fi
    ${tools_dir}/bufr_compare ${data_dir}/bufr/$file $tempBufr

    TEMP_OUT1=${label}.$file.dump.out
    TEMP_OUT2=${label}.$tempBufr.dump.out
    ${tools_dir}/bufr_dump -p ${data_dir}/bufr/$file > $TEMP_OUT1
    ${tools_dir}/bufr_dump -p $tempBufr              > $TEMP_OUT2
    diff $TEMP_OUT1 $TEMP_OUT2
    rm -f $TEMP_OUT1 $TEMP_OUT2
  fi

  rm -f $tempExe $tempSrc $tempBufr
done

cd $test_dir
rm -fr $tempDir

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
label="bufr_dump_encode_python_test"

#Create log file
fLog=${label}".log"
rm -f $fLog

tempBufr=outfile.bufr
tempDir=${label}.dir
mkdir -p $tempDir
cd $tempDir

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`

for file in ${bufr_files}
do
  inputBufr=${data_dir}/bufr/$file
  tempSrc=$label.$file.py

  # Too large for this test
  if [ "$file" = "ias1_240.bufr" ]; then
    continue
  fi

  # Generate Python code from BUFR file
  ${tools_dir}/bufr_dump -Epython $inputBufr > $tempSrc
  # The python code always creates an output file called outfile.bufr
  $PYTHON $tempSrc

  # Check original BUFR file against one we generated from sample
  ${tools_dir}/bufr_compare $inputBufr $tempBufr

  TEMP_OUT1=${label}.$file.dump.out
  TEMP_OUT2=${label}.$tempBufr.dump.out
  ${tools_dir}/bufr_dump -p $inputBufr > $TEMP_OUT1
  ${tools_dir}/bufr_dump -p $tempBufr  > $TEMP_OUT2
  diff $TEMP_OUT1 $TEMP_OUT2
  rm -f $TEMP_OUT1 $TEMP_OUT2

  rm -f $tempSrc $tempBufr
done

cd $test_dir
rm -fr $tempDir

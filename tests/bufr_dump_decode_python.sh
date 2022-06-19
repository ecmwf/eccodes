#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

# Define a common label for all the tmp files
label="bufr_dump_decode_python_test"

# Create log file
fLog=temp.${label}".log"
rm -f $fLog

tempDir=temp.${label}.dir
mkdir -p $tempDir
cd $tempDir

echo "PYTHONPATH set to $PYTHONPATH"

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`

for file in ${bufr_files}
do
  inputBufr=${data_dir}/bufr/$file
  tempSrc=temp.$label.$file.py

  # Too large for this test
  if [ "$file" = "ias1_240.bufr" ]; then
    continue
  fi

  # Generate Python code from BUFR file
  ${tools_dir}/bufr_dump -Dpython $inputBufr > $tempSrc

  if test "x$PYTHON" != "x"; then
    $PYTHON $tempSrc $inputBufr
  fi
  rm -f $tempSrc
done

cd $test_dir
rm -fr $tempDir

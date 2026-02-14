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

label="grib_check_concept_keys_test"
temp=temp.$label.txt


if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

keys_file="$proj_dir/tests/keys"
if [ ! -f "$keys_file" ]; then
   echo "The file $keys_file does not exist!"
   exit 1
fi

files=$( find "$proj_dir/definitions/grib2" -name '*Concept*def' -print )

file_count=0
for f in $files; do
   file_count=$((file_count + 1))
   list=$( ${test_dir}/grib_check_param_concepts -v dummy $f |sort -u )
   for k in $list; do
      set +e
      grep -q -w $k "$keys_file"
      if [ $? -ne 0 ]; then
         echo "ERROR on key $k (See $f)" 2>&1
         exit 1
      fi
      set -e
   done
done
echo "Checked $file_count files."


# Clean up
rm -f $temp

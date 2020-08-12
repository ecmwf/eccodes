# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
set -ea

#
# Utility functions for testing
#

# Check the key(s) in the given grib file have the expected value(s)
# Assumes the file has only ONE message otherwise output goes on to
# several lines
grib_check_key_equals()
{
   a_file=$1
   a_key=$2
   a_expected=$3
   a_result=`${tools_dir}/grib_get -p $a_key $a_file`
   if [ "$a_result" != "$a_expected" ]; then
      echo "File:     $a_file"
      echo "Key(s):   $a_key"
      echo "Expected: $a_expected"
      echo "Result:   $a_result"
      exit 1
   fi
}

grib_check_key_exists()
{
   a_file=$1
   a_key=$2
   # grib_get will fail if the key is not found
   $tools_dir/grib_get -p $a_key $a_file >/dev/null
}

grib_check_filesize()
(
  # There are tests that try to work on non-existing file.
  # e.g. eccodes_t_grib_bitsPerValue. Check the size if it exists.
  fullpath=${data_dir}/$1
  if [ -f $fullpath ]; then
    # try GNU version first
    set +e
    realsize=`stat -c %s $fullpath`
    if [ $? != 0 ]; then
      set -e
      realsize=`stat -f %z $fullpath`
    else
      set -e
    fi
    dir=`dirname $fullpath`;
    expected=`grep " $1$" $dir/filesize_db.txt | awk -F " " '{print $1}'`
    if [ "$realsize" != "$expected" ]; then
      echo Data file \"$1\" does not have the expected size, $expected, which is from http://download.ecmwf.org/test-data/eccodes/data/.
      exit 1
    fi
  fi
)

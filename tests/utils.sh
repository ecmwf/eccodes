# Copyright 2005-2016 ECMWF.
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
   a_result=`${tools_dir}grib_get -p $a_key $a_file`
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

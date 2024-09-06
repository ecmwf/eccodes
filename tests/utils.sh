#!/bin/sh
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
      echo "File:     '$a_file'"
      echo "Key(s):   '$a_key'"
      echo "Expected: '$a_expected'"
      echo "Result:   '$a_result'"
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

# This is useful in situations where we intend a key to have
# a single value (size=1). For example when we redefine a transient
grib_check_key_is_scalar()
{
   a_file=$1
   a_key=$2
   a_val=$3
   result=$(echo "print '|[$a_key]|';" | ${tools_dir}/grib_filter - $a_file)
    [ "$result" = "|$a_val|" ]
}

# This ensures setting the keys via grib_filter (similar to the API, one key at a time)
# works the same way as grib_set. The resultant GRIB outputs are compared
# E.g.,
#    grib_encode_via_set_and_filter $label key1=val1,key2=val2,key3=val3 $ECCODES_SAMPLES_PATH/GRIB2.tmpl
#
grib_encode_via_set_and_filter()
{
   _label=$1 # For generating temp files
   _kvals=$2 # key1=val1,key2=val2
   _input=$3 # Must be a GRIB file

   temp_encode_filt=temp.encode.$_label.filt
   temp_encode_gribA=temp.encode.$_label.A.grib
   temp_encode_gribB=temp.encode.$_label.B.grib

   for kv in $(echo $_kvals | tr ',' ' '); do
      k=$(echo $kv | awk -F= '{print $1}')
      v=$(echo $kv | awk -F= '{print $2}')
      if [[ "$v" =~ ^[A-z] ]]; then
         echo "set $k = \"$v\" ;" >> $temp_encode_filt
      else
         echo "set $k = $v ;" >> $temp_encode_filt
      fi
   done
   echo "write;" >> $temp_encode_filt
   # cat $temp_encode_filt
   ${tools_dir}/grib_filter -o $temp_encode_gribA $temp_encode_filt $_input
   ${tools_dir}/grib_set -s $_kvals $_input $temp_encode_gribB
   ${tools_dir}/grib_compare  $temp_encode_gribA $temp_encode_gribB
   rm -f $temp_encode_filt $temp_encode_gribB $temp_encode_gribA
}

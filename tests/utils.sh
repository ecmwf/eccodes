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
check_key_equals()
{
   file=$1
   key=$2
   val=$3
   result=`${tools_dir}grib_get -p $key $file`
   [ "$result" = "$val" ]
}


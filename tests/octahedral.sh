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

REDIRECT=/dev/null

this_test="octa_grid.test"
temp=temp.$this_test

input=${data_dir}/msl.octa.glob.grib1

# Check 'global' key
grib_check_key_equals $input "global,isOctahedral" "1 1"

# Check numberOfDataPoints
grib_check_key_equals $input "numberOfDataPoints,numberOfCodedValues" "6599680 6599680"

# Iterator
${tools_dir}grib_get_data $input > $temp
numlines=`wc -l $temp | awk '{print $1}'`
[ "$numlines" = "6599681" ]     # 1 + numberOfDataPoints

rm -f $temp

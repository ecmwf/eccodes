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

#set -x

#Enter data dir
cd ${data_dir}

#Define a common label for all the tmp files
label="grib_copy_test"
temp=${label}".grib.tmp"

#-------------------------------------------------------------------
# Test: The -X option
#-------------------------------------------------------------------
echo "Test: use of -X option"
input=tigge_pf_ecmwf.grib2
${tools_dir}/grib_copy -w count=1 -X 0 $input $temp #First msg
r1=`${tools_dir}/grib_get -w count=1 -n ls $input`
r2=`${tools_dir}/grib_get -n ls $temp`
[ "$r1" = "$r2" ]

#${tools_dir}/grib_copy -w count=1 -X 57143 $input $temp #Last msg
#r1=`${tools_dir}/grib_get -w count=37 -n ls $input`
#r2=`${tools_dir}/grib_get -n ls $temp`
#[ "$r1" = "$r2" ]

# Get two messages
#${tools_dir}/grib_copy -w count=1/2 -X 25071 $input $temp
#count=`${tools_dir}/grib_count $temp`
#[ $count -eq 2 ]

#r=`${tools_dir}/grib_get -w count=1 -p typeOfLevel,level,shortName $temp`
#[ "$r" = "heightAboveGround 2 mx2t6" ]
#r=`${tools_dir}/grib_get -w count=2 -p typeOfLevel,level,shortName $temp`
#[ "$r" = "heightAboveGround 2 mn2t6" ]

# Clean up
#-----------
rm -f $temp

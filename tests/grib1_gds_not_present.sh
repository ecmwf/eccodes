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

REDIRECT=/dev/null

label="grib1_gds_not_present_test"
tempGrib=temp.$label.grib

input=$data_dir/reduced_latlon_surface.grib1
${tools_dir}/grib_set -s gridDescriptionSectionPresent=0 $input $tempGrib
result=`${tools_dir}/grib_get -fp Ni,Nj $tempGrib`
[ "$result" = "not_found not_found" ]

${tools_dir}/grib_dump $tempGrib > $REDIRECT 2>&1
# ${tools_dir}/grib_get -n statistics $tempGrib

max=`${tools_dir}/grib_get -F%.3f -p max $input`
min=`${tools_dir}/grib_get -F%.3f -p min $input`
[ "$max" = "12.597" ]
[ "$min" = "0.019" ]

# Clean up
rm -f $tempGrib

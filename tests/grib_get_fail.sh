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

label="grib_get_fail_test"
tempText=temp.$label.txt
REDIRECT=/dev/null

# Check input file has been downloaded
[ -f ${data_dir}/regular_latlon_surface.grib1 ]

# Expect failure as the key does not exist
set +e
${tools_dir}/grib_get -p boomerang ${data_dir}/regular_latlon_surface.grib1 2> $REDIRECT > $REDIRECT
if [ $? -eq 0 ] ; then
  exit 1;  # Should not have succeeded
fi
set -e

# ECC-1551: Print which key does not exist
set +e
${tools_dir}/grib_get -p Ni,Nh,Nj $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Nh (Key/value not found)" $tempText

rm -f $tempText

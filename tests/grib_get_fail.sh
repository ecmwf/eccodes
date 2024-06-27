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

# Check input file has been downloaded
[ -f ${data_dir}/regular_latlon_surface.grib1 ]

# Expect failure as the key does not exist
set +e
${tools_dir}/grib_get -p boomerang ${data_dir}/regular_latlon_surface.grib1
status=$?
set -e
[ $status -ne 0 ]

# ECC-1551: Print which key does not exist
# -----------------------------------------
set +e
${tools_dir}/grib_get -p Ni,Nh,Nj $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Nh (Key/value not found)" $tempText


# Nearest
# ---------
set +e
${tools_dir}/grib_get -l abc $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Wrong latitude value" $tempText

set +e
${tools_dir}/grib_get -s Nj=MISSING -l 0,0,1 $ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib1.tmpl > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Key Nj cannot be 'missing'" $tempText

set +e
${tools_dir}/grib_get -s Nj=0 -l 0,0,1 $ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib1.tmpl > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Key Nj cannot be 0" $tempText


set +e
${tools_dir}/grib_get -l 0,0,5 $ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib2.tmpl > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Wrong mode given" $tempText


set +e
${tools_dir}/grib_get -l 0,0,1,nonexistingmask $ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib1.tmpl > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
cat $tempText
grep -q "unable to open mask file" $tempText


# ------------------------
# Unreadable message
# ------------------------
outfile=temp.$label.out
echo GRIB > $outfile
set +e
${tools_dir}/grib_get -p edition $outfile /dev/null > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "unreadable message" $tempText
rm -f $outfile


# ----------------------
# Wrong message type
# ----------------------
set +e
${tools_dir}/grib_get -p edition $ECCODES_SAMPLES_PATH/BUFR3.tmpl > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Input file seems to be BUFR" $tempText


# Clean up
rm -f $tempText

#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label="grib_check_message_validity_test"
tempGrib=temp.$label.grib
tempText=temp.$label.txt
tempFilt=temp.$label.filt

grib_check_key_equals $ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl isMessageValid 1
grib_check_key_equals $ECCODES_SAMPLES_PATH/GRIB2.tmpl                  isMessageValid 1
grib_check_key_equals $ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib1.tmpl   isMessageValid 1
grib_check_key_equals $ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib2.tmpl   isMessageValid 1
grib_check_key_equals $ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl            isMessageValid 1

IFS_SAMPLES_ROOT=${proj_dir}/ifs_samples
grib_check_key_equals $IFS_SAMPLES_ROOT/grib1_mlgrib2_ccsds/gg_ml.tmpl        isMessageValid 1
grib_check_key_equals $IFS_SAMPLES_ROOT/grib1_mlgrib2_ccsds/gg_sfc_grib2.tmpl isMessageValid 1
grib_check_key_equals $IFS_SAMPLES_ROOT/grib1_mlgrib2_ccsds/sh_ml.tmpl        isMessageValid 1

# Do it with debug enabled
ECCODES_DEBUG=-1  ${tools_dir}/grib_get -p isMessageValid $ECCODES_SAMPLES_PATH/GRIB2.tmpl


# Bad sections
# ---------------
# Correct key order: NV=6,PVPresent=1
${tools_dir}/grib_set -s PVPresent=1,NV=6 $ECCODES_SAMPLES_PATH/reduced_gg_pl_128_grib2.tmpl $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Section 5 is missing" $tempText
if [ $HAVE_GEOGRAPHY -eq 1 ]; then
   grep -q "Error instantiating iterator gaussian_reduced" $tempText
fi

# Bad grib2 surface keys
# -----------------------
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s scaleFactorOfSecondFixedSurface=99 $sample $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Second fixed surface: If the type of surface is missing so should its scaleFactor/scaledValue keys" $tempText

${tools_dir}/grib_set -s typeOfFirstFixedSurface=missing,scaleFactorOfFirstFixedSurface=99 $sample $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "First fixed surface: If the type of surface is missing so should its scaleFactor/scaledValue keys" $tempText

${tools_dir}/grib_set -s scaledValueOfSecondFixedSurface=0,scaleFactorOfSecondFixedSurface=missing,typeOfSecondFixedSurface=1 $sample $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Second fixed surface: If the scale factor is missing so should the scaled value and vice versa" $tempText

# Some of our grib2 test data have problems!
input=$data_dir/test_uuid.grib2
result=$( ${tools_dir}/grib_get -w count=1 -p isMessageValid $input 2>$tempText )
[ $result -eq 0 ]
grep -q "Second fixed surface: If the scale factor is missing so should the scaled value and vice versa" $tempText

input=$data_dir/missing.grib2
result=$( ${tools_dir}/grib_get -w count=1 -p isMessageValid $input 2>$tempText )
[ $result -eq 0 ]
grep -q "Second fixed surface: If the type of surface is missing so should its scaleFactor/scaledValue keys" $tempText

# Some surface types require sv/sf to be set (not missing)
${tools_dir}/grib_set -s typeOfFirstFixedSurface=160 $sample $tempGrib
grib_check_key_equals $tempGrib scaledValueOfFirstFixedSurface MISSING
grib_check_key_equals $tempGrib scaleFactorOfFirstFixedSurface MISSING
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "First fixed surface: Type 160 .Depth below sea level. requires a level" $tempText

# Some surface types have no units
${tools_dir}/grib_set -s scaleFactorOfFirstFixedSurface=0,scaledValueOfFirstFixedSurface=9,typeOfFirstFixedSurface=1 $sample $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "First fixed surface: .*scaleFactor/scaledValue keys must be set to missing" $tempText


# Check paramId is mapped
# ------------------------------
input=$data_dir/tigge_cf_ecmwf.grib2
# Message 43 in this file has a deprecated mapping (wilting point)
result=$( ${tools_dir}/grib_get -w count=43 -p isMessageValid $input 2>$tempText )
[ $result -eq 0 ]
grep -q "parameter is not mapped" $tempText


# Check steps
# ------------------------------
${tools_dir}/grib_set -s stepType=accum,startStep=12,endStep=6  $ECCODES_SAMPLES_PATH/GRIB1.tmpl $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Invalid step: startStep > endStep" $tempText

# Wrong order of keys
${tools_dir}/grib_set -s startStep=0,endStep=0,stepType=accum  $ECCODES_SAMPLES_PATH/GRIB1.tmpl $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Invalid steps: stepType=accum but startStep=endStep" $tempText


# Check regular lat/lon
# ------------------------------
if [ $HAVE_GEOGRAPHY -eq 1 ]; then
   ${tools_dir}/grib_set -s Nj=0 $data_dir/sample.grib2 $tempGrib
   grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
   grep -q "Regular grid Geoiterator" $tempText
fi

# Check reduced Gaussian grid Ni
# ------------------------------
sample=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
cat >$tempFilt<<EOF
   set Ni = 0; # illegal
   assert ( isMessageValid == 0 );
   set Ni = MISSING;
   assert ( isMessageValid == 1 );
   set Ni = 0; # illegal again
   write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample 2>$tempText
grep -q "Invalid Ni" $tempText
grib_check_key_equals $tempGrib isMessageValid 0
grib_check_key_equals $sample   isMessageValid 1

# Check gridType and packingType
# ------------------------------
${tools_dir}/grib_set -s packingType=grid_simple $ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Mismatch between gridType .* and packingType" $tempText


# Check reduced Gaussian grid pl
# ------------------------------
sample=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
cat >$tempFilt<<EOF
   meta pl_elem4 element(pl, 4);
   set pl_elem4 = 0;
   assert ( isMessageValid == 0 );
   write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample 2>$tempText
grep -q "Invalid PL array" $tempText
grib_check_key_equals $tempGrib isMessageValid 0


# Check data values
# ------------------------------
${tools_dir}/grib_set -s bitsPerValue=25 $data_dir/sample.grib2 $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Data section size mismatch" $tempText


# Check number of values, missing etc
# -----------------------------------
${tools_dir}/grib_set -s values=5,numberOfDataPoints=55 $data_dir/sample.grib2 $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "numberOfCodedValues + numberOfMissing != numberOfDataPoints" $tempText


# Clean up
rm -f $tempGrib $tempText $tempFilt

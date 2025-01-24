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
grib_check_key_equals $ECCODES_SAMPLES_PATH/GRIB2.tmpl isMessageValid 1


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
grep -q "Second fixed surface: If the type of surface is missing so should its scaled keys" $tempText

${tools_dir}/grib_set -s typeOfFirstFixedSurface=missing,scaleFactorOfFirstFixedSurface=99 $sample $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "First fixed surface: If the type of surface is missing so should its scaled keys" $tempText

${tools_dir}/grib_set -s scaledValueOfSecondFixedSurface=0,scaleFactorOfSecondFixedSurface=missing,typeOfSecondFixedSurface=1 $sample $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Second fixed surface: If the scale factor is missing so should the scaled value and vice versa" $tempText


# Check steps
# ------------------------------
${tools_dir}/grib_set -s stepType=accum,startStep=12,endStep=6  $ECCODES_SAMPLES_PATH/GRIB1.tmpl $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Invalid step: startStep > endStep" $tempText


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


# Clean up
rm -f $tempGrib $tempText $tempFilt

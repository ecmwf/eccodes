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

# ---------------------------------------------------------------------
# A GRIB2 message whose section 5 numberOfValues is smaller than the number
# of points present in the bitmap must be rejected with a clear message.
# It used to read past the end of the coded values (heap-buffer-overflow).
# ---------------------------------------------------------------------

label="grib_bitmap_coded_values_test"

tempGrib=temp.$label.grib
tempPack=temp.$label.pack.grib
tempBad=temp.$label.bad.grib
tempOut=temp.$label.out

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# A message with a bitmap in which all the points are present
${tools_dir}/grib_set -r -s bitsPerValue=16 $sample_grib2 $tempPack
${tools_dir}/grib_set -r -s bitmapPresent=1 $tempPack $tempGrib
grib_check_key_equals $tempGrib bitmapPresent,numberOfMissing '1 0'

# Patch section 5 numberOfValues (octets 6..9 of the section) down to 1
offset=$(${tools_dir}/grib_get -p offsetSection5 $tempGrib)
{
    head -c $((offset + 5)) $tempGrib
    printf '\000\000\000\001'
    tail -c +$((offset + 10)) $tempGrib
} > $tempBad

set +e
${tools_dir}/grib_get_data $tempBad > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]

# The message must say what is wrong and which keys are inconsistent
grep -q "too few coded values for the bitmap" $tempOut
grep -q "codedValues" $tempOut
grep -q "bitmap" $tempOut

rm -f $tempGrib $tempPack $tempBad $tempOut

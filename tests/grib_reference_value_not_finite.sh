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

label="grib_reference_value_not_finite_test"
temp1=temp1.$label.grib
temp2=temp2.$label.grib
tempText=temp.$label.txt

# A reference value whose IEEE bits are infinity or NaN (exponent 255) was decoded with an index
# beyond the exponent table. Reading it must give inf/nan, decoding the values must fail.
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

for packing in grid_simple grid_ccsds; do
    ${tools_dir}/grib_set -r -s packingType=$packing,bitsPerValue=16 $sample $temp1
    offset=$( ${tools_dir}/grib_get -p offsetSection5 $temp1 )
    offset=$((offset + 11))

    # +inf, -inf and a NaN
    for bytes in '\177\200\000\000' '\377\200\000\000' '\177\300\000\000'; do
        cp $temp1 $temp2
        printf "$bytes" | dd of=$temp2 bs=1 seek=$offset conv=notrunc 2>/dev/null

        ${tools_dir}/grib_get -p referenceValue $temp2 > $tempText
        grep -q -i -E "inf|nan" $tempText

        set +e
        ${tools_dir}/grib_get -p max $temp2 > $tempText 2>&1
        status=$?
        set -e
        [ $status -ne 0 ]
        grep -q "referenceValue" $tempText
    done
done

rm -f $temp1 $temp2 $tempText

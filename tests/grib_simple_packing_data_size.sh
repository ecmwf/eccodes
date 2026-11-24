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

# A simple packed message whose data section is empty must decode as a constant field (the
# decoder read beyond the end of the data), and one whose data section is too short must be
# rejected

label="grib_simple_packing_data_size_test"
tempGrib=temp.$label.grib
tempLog=temp.$label.log
infile=${data_dir}/regular_latlon_surface.grib2

# Write the big-endian integer $1 as $2 bytes
write_be() {
    v=$1; n=$2; out=""
    while [ $n -gt 0 ]; do
        out="$(printf '\\%03o' $((v % 256)))$out"
        v=$((v / 256)); n=$((n - 1))
    done
    printf "$out"
}

# Keep the message up to the header of section 7, set the length of section 7 to 5 (no data)
off7=$(${tools_dir}/grib_get -p offsetSection7 $infile)
newLength=$((off7 + 5 + 4))
{
    head -c 8 $infile
    write_be $newLength 8
    tail -c +17 $infile | head -c $((off7 - 16))
    write_be 5 4
    printf '\007'
    printf '7777'
} > $tempGrib

ref=$(${tools_dir}/grib_get -p referenceValue $tempGrib)
for impl in default new; do
    if [ $impl = new ]; then
        export ECCODES_USE_SP=1
    fi
    # Empty data section: all values are the reference value
    stats=$(${tools_dir}/grib_get -F%.6g -p min,max $tempGrib)
    [ "$stats" = "$(printf '%.6g %.6g' $ref $ref)" ]

    # Data section of 1 byte (496 values need 992 bytes)
    {
        head -c 8 $tempGrib
        write_be $((newLength + 1)) 8
        tail -c +17 $tempGrib | head -c $((off7 - 16))
        write_be 6 4
        printf '\007\001'
        printf '7777'
    } > $tempGrib.short
    set +e
    ${tools_dir}/grib_get_data $tempGrib.short > $tempLog 2>&1
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "Data section size mismatch" $tempLog
    unset ECCODES_USE_SP
done

rm -f $tempGrib $tempGrib.short $tempLog

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

label="grib2_section_length_corrupt_test"
temp1=temp1.$label.grib
temp2=temp2.$label.grib
tempText=temp.$label.txt

# A GRIB2 section length reaching beyond the end of the message (or shorter than a section
# header) corrupted the heap when the message was rebuilt in multi-field mode (the tools crashed).
# It must be reported as an error.
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

for packing in grid_simple grid_ccsds; do
    if [ $packing = grid_ccsds ] && [ $HAVE_AEC -eq 0 ]; then continue; fi
    ${tools_dir}/grib_set -r -s packingType=$packing,bitsPerValue=16 $sample $temp1
    offset=$( ${tools_dir}/grib_get -p offsetSection7 $temp1 )
    length=$( ${tools_dir}/grib_get -p section7Length $temp1 )

    for delta in 1 100 -8; do
        newLength=$((length + delta))
        cp $temp1 $temp2
        # Section length: 4 bytes, big endian
        printf "$(printf '\\%03o\\%03o\\%03o\\%03o' $((newLength >> 24 & 255)) $((newLength >> 16 & 255)) $((newLength >> 8 & 255)) $((newLength & 255)))" |
            dd of=$temp2 bs=1 seek=$offset conv=notrunc 2>/dev/null

        for tool in grib_get_data grib_dump grib_ls; do
            set +e
            ${tools_dir}/$tool $temp2 > $tempText 2>&1
            status=$?
            set -e
            # An error, not a crash (killed by SIGABRT, SIGBUS, SIGFPE, SIGKILL or SIGSEGV)
            [ $status -ne 0 ]
            case $status in 134|135|136|137|139) exit 1 ;; esac
            grep -q -i "length" $tempText
        done
    done
done

rm -f $temp1 $temp2 $tempText

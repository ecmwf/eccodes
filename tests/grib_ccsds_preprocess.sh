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

# CCSDS data without preprocessor (ccsdsFlags without AEC_DATA_PREPROCESS, bit 8; CCSDS 121.0-B-3,
# 4.1.1): with ECCODES_USE_SPAEC=1, and without libaec, SpAec must code it. The values must be
# those of the field packed with preprocessor, and the message that of libaec.

label="grib_ccsds_preprocess_test"
infile=${data_dir}/ccsds.grib2
temp=temp.$label.grib2
tempRef=temp.$label.ref.grib2
tempAec=temp.$label.aec.grib2

# ccsdsFlags 6: no preprocessor (AEC_DATA_3BYTE, AEC_DATA_MSB); 22: also AEC_RESTRICTED
for case in "6 12" "6 24" "22 4"; do
    set -- $case
    flags=$1
    bpv=$2

    # The field packed with preprocessor (ccsdsFlags 14) and the same bits per value
    ${tools_dir}/grib_set -r -s bitsPerValue=$bpv,ccsdsFlags=14 $infile $tempRef

    ECCODES_USE_SPAEC=1 ${tools_dir}/grib_set -r -s bitsPerValue=$bpv,ccsdsFlags=$flags $infile $temp
    [ "$(${tools_dir}/grib_get -p ccsdsFlags,bitsPerValue $temp)" = "$flags $bpv" ]
    ECCODES_USE_SPAEC=1 ${tools_dir}/grib_compare -c data:n $tempRef $temp

    if [ $HAVE_AEC -eq 1 ]; then
        # libaec writes the same message and decodes it alike
        ${tools_dir}/grib_set -r -s bitsPerValue=$bpv,ccsdsFlags=$flags $infile $tempAec
        cmp $temp $tempAec
        ${tools_dir}/grib_compare -c data:n $tempRef $tempAec
    fi
done

rm -f $temp $tempRef $tempAec

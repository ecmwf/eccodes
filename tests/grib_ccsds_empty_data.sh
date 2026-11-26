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

label="grib_ccsds_empty_data_test"
tempText=temp.$label.txt

# A CCSDS message with an empty data section (bitsPerValue > 0, section 7 has no data) decodes
# as a constant field of the reference value with libaec: SpAec must decode it the same way
input=${data_dir}/lcgcr/regular_ll_sttd.grib2
[ "$( ${tools_dir}/grib_get -p section7Length $input )" -eq 5 ]

${tools_dir}/grib_get -p min,max,average $input > $tempText
grep -q "^273 273 273$" $tempText

if [ $HAVE_AEC -eq 1 ]; then
    ECCODES_USE_SPAEC=1 ${tools_dir}/grib_get -p min,max,average $input > $tempText
    grep -q "^273 273 273$" $tempText
fi

rm -f $tempText

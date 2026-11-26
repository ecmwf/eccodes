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

# CCSDS packing that fails reports the error of the legacy code also with ECCODES_USE_SPAEC=1 (and
# without libaec): too many bits per value, and a range too small for any binary scale factor

label="grib_ccsds_error_codes_test"
sample=${samp_dir}/ccsds_grib2.tmpl
temp=temp.$label.grib2
tempFilter=temp.$label.filter
tempErr=temp.$label.err

check_error()
{
    expected=$1
    for spaec in 0 1; do
        set +e
        ECCODES_USE_SPAEC=$spaec ${tools_dir}/grib_filter -o $temp $tempFilter $sample 2> $tempErr
        status=$?
        set -e
        [ $status -ne 0 ]
        grep -q "ERROR: $expected" $tempErr
    done
}

# 64 bits per value
echo 'set bitsPerValue=64; set values={1,2,3,4,5,6,7,8.5}; write;' > $tempFilter
check_error "Value out of coding range"

# A range too small for 60 bits per value with the binary scale factor of at least -127
echo 'set bitsPerValue=60; set decimalScaleFactor=-33; set values={1,1,1,1,1,1,1,1.0000001}; write;' > $tempFilter
check_error "Underflow"

rm -f $temp $tempFilter $tempErr

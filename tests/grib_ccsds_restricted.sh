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

# CCSDS data with the restricted set of code options (AEC_RESTRICTED, ccsdsFlags 12 + 16 = 28):
# SpAec must encode and decode it itself (it was left to libaec), like libaec: the restricted set
# for 1 to 4 bits per value, rejected for 5 to 8 bits, the basic set for more bits. With libaec the
# messages and the values must be identical.

label="grib_ccsds_restricted_test"
infile=${data_dir}/ccsds.grib2
tempSpaec=temp.$label.spaec.grib2
tempAec=temp.$label.aec.grib2
tempLog=temp.$label.log

for bpv in 1 2 3 4 12; do
    ECCODES_DEBUG=1 ECCODES_USE_SPAEC=1 ${tools_dir}/grib_set -r -s ccsdsFlags=28,bitsPerValue=$bpv $infile $tempSpaec 2> $tempLog
    grep -q "packing codedValues.*(SpAec)" $tempLog
    [ $(${tools_dir}/grib_get -p ccsdsFlags,bitsPerValue $tempSpaec) = "28 $bpv" ]

    ECCODES_DEBUG=1 ECCODES_USE_SPAEC=1 ${tools_dir}/grib_get -F%.6g -p max,min,avg $tempSpaec > $tempLog.values 2> $tempLog
    grep -q "unpacking codedValues.*(SpAec)" $tempLog

    if [ $HAVE_AEC -eq 1 ]; then
        ${tools_dir}/grib_set -r -s ccsdsFlags=28,bitsPerValue=$bpv $infile $tempAec
        cmp $tempSpaec $tempAec
        [ "$(${tools_dir}/grib_get -F%.6g -p max,min,avg $tempAec)" = "$(cat $tempLog.values)" ]
    fi
done

# 5 to 8 bits per value: no restricted set (rejected like libaec does)
set +e
ECCODES_USE_SPAEC=1 ${tools_dir}/grib_set -r -s ccsdsFlags=28,bitsPerValue=6 $infile $tempSpaec 2> $tempLog
status=$?
set -e
[ $status -ne 0 ]

rm -f $tempSpaec $tempAec $tempLog $tempLog.values

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

# CCSDS data with AEC_NOT_ENFORCE (ccsdsFlags bit 64) and a block size other than 8, 16, 32 or 64:
# libaec encodes and decodes it, so it must also work with ECCODES_USE_SPAEC=1 (SpAec supports
# only the standard block sizes: such data must be left to libaec, not rejected)

label="grib_ccsds_not_enforce_test"
infile=${data_dir}/ccsds.grib2
tempAec=temp.$label.aec.grib2
tempSpaec=temp.$label.spaec.grib2

# ccsdsFlags 72: AEC_DATA_PREPROCESS and AEC_NOT_ENFORCE, block size 4
${tools_dir}/grib_set -r -s ccsdsFlags=72,ccsdsBlockSize=4 $infile $tempAec
[ "$(${tools_dir}/grib_get -p ccsdsFlags,ccsdsBlockSize $tempAec)" = "72 4" ]

# Decoding with SpAec selected gives the values of libaec
expected=$(${tools_dir}/grib_get -F%.17g -p max,min,avg $tempAec)
result=$(ECCODES_USE_SPAEC=1 ${tools_dir}/grib_get -F%.17g -p max,min,avg $tempAec)
[ "$result" = "$expected" ]

# Encoding with SpAec selected gives the message of libaec
ECCODES_USE_SPAEC=1 ${tools_dir}/grib_set -r -s ccsdsFlags=72,ccsdsBlockSize=4 $infile $tempSpaec
cmp $tempAec $tempSpaec

rm -f $tempAec $tempSpaec

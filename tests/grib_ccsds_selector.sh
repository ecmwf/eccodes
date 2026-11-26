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

# ECCODES_SPAEC_SELECTOR: the split-sample selector of SpAec (ECCODES_USE_SPAEC=1, or without
# libaec). search (the default): the data of libaec. estimate: other data (a few bytes more) with
# the same values. Another value: encoding fails, decoding works.

label="grib_ccsds_selector_test"
infile=${data_dir}/ccsds_szip.grib2
tempSearch=temp.$label.search.grib2
tempDefault=temp.$label.default.grib2
tempEstimate=temp.$label.estimate.grib2
tempAec=temp.$label.aec.grib2
tempSimple1=temp.$label.simple1.grib2
tempSimple2=temp.$label.simple2.grib2
tempLog=temp.$label.log

# search: the default
ECCODES_USE_SPAEC=1 ECCODES_SPAEC_SELECTOR=search ${tools_dir}/grib_set -r -s bitsPerValue=16 $infile $tempSearch
ECCODES_USE_SPAEC=1 ${tools_dir}/grib_set -r -s bitsPerValue=16 $infile $tempDefault
cmp $tempSearch $tempDefault
if [ $HAVE_AEC -eq 1 ]; then
    ECCODES_USE_SPAEC=0 ${tools_dir}/grib_set -r -s bitsPerValue=16 $infile $tempAec
    cmp $tempSearch $tempAec
fi

# estimate: other data, the same values (the same message after repacking with simple packing)
ECCODES_USE_SPAEC=1 ECCODES_SPAEC_SELECTOR=estimate ${tools_dir}/grib_set -r -s bitsPerValue=16 $infile $tempEstimate
set +e
cmp -s $tempSearch $tempEstimate
status=$?
set -e
[ $status -ne 0 ]
ECCODES_USE_SPAEC=1 ${tools_dir}/grib_set -r -s packingType=grid_simple $tempSearch $tempSimple1
ECCODES_USE_SPAEC=1 ${tools_dir}/grib_set -r -s packingType=grid_simple $tempEstimate $tempSimple2
cmp $tempSimple1 $tempSimple2
if [ $HAVE_AEC -eq 1 ]; then
    # libaec decodes the data of estimate (standard CCSDS 121.0-B-3)
    ECCODES_USE_SPAEC=0 ${tools_dir}/grib_set -r -s packingType=grid_simple $tempEstimate $tempSimple2
    cmp $tempSimple1 $tempSimple2
fi

# Another value: encoding fails with a message naming the variable, decoding works
set +e
ECCODES_USE_SPAEC=1 ECCODES_SPAEC_SELECTOR=fast ${tools_dir}/grib_set -r -s bitsPerValue=16 $infile $tempDefault 2> $tempLog
status=$?
set -e
[ $status -ne 0 ]
grep -q "ECCODES_SPAEC_SELECTOR" $tempLog
ECCODES_USE_SPAEC=1 ECCODES_SPAEC_SELECTOR=fast ${tools_dir}/grib_get -p avg $tempEstimate

rm -f $tempSearch $tempDefault $tempEstimate $tempAec $tempSimple1 $tempSimple2 $tempLog

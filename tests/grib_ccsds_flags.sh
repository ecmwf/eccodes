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

# CCSDS data with options SpAec does not implement (here: no preprocessing) must be decoded
# like libaec does, not silently decoded with the wrong options

label="grib_ccsds_flags_test"
infile=${data_dir}/ccsds.grib2
tempGrib=temp.$label.grib2

# Pack the data with libaec without preprocessing (ccsdsFlags=6)
ECCODES_USE_LIBAEC=1 ${tools_dir}/grib_set -r -s ccsdsFlags=6 $infile $tempGrib
[ $(${tools_dir}/grib_get -p ccsdsFlags $tempGrib) = 6 ]

expected=$(ECCODES_USE_LIBAEC=1 ${tools_dir}/grib_get -F%.6g -p max,min,avg $tempGrib)
result=$(${tools_dir}/grib_get -F%.6g -p max,min,avg $tempGrib)
echo "libaec: $expected, default: $result"
[ "$result" = "$expected" ]

rm -f $tempGrib

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

label="wmo_read_any_from_stream_test"
tempText=temp.$label.txt
tempGrib=temp.$label.grib

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

expected="125 254 383 512 "
actual=$(${test_dir}/wmo_read_any_from_stream_with_offsets $data_dir/gts.grib)
# compare expected and actual
if [ "$expected" != "$actual" ]; then
    echo "ERROR: $label"
    echo "expected:$expected"
    echo "actual:  $actual"
    exit 1
fi
# Clean up
rm -f $tempText $tempGrib

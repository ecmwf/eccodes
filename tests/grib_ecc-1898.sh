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

label="grib_ecc-1898_test"

tempGrib=temp.$label.grib
tempOut=temp.${label}.txt

$tools_dir/grib_dump -Da $samp_dir/reduced_gg_pl_32_grib2.tmpl > $tempOut
grep -q '288-288 g2level level = 1000 \[vertical.level, mars.levelist\]' $tempOut

# For given MARS stream/types, we unalias the mars.levelist key
$tools_dir/grib_set -s stream=gfas,type=gsd $samp_dir/reduced_gg_pl_32_grib2.tmpl $tempGrib

# Check that the key is unaliased
$tools_dir/grib_dump -Da $tempGrib > $tempOut
grep -q '288-288 g2level level = 1000 \[vertical.level\]' $tempOut

$tools_dir/grib_dump -p levelist -Da $tempGrib 2> $tempOut
grep -q 'levelist: Key/value not found' $tempOut

result=$($tools_dir/grib_get -fp levelist $tempGrib)
[ "$result" = "not_found" ]

result=$($tools_dir/grib_get -fp mars.levelist $tempGrib)
[ "$result" = "not_found" ]

# Clean up
rm -f $tempGrib $tempOut

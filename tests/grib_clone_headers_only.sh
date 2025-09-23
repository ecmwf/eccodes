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

label="grib_clone_headers_only_test"

tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib

inputs="
  sst_globus0083.grib
  mixed.grib
"

for f in $inputs; do
    infile=$data_dir/$f
    rm -f $tempGribA $tempGribB
    $EXEC ${test_dir}/grib_clone_headers_only $infile $tempGribA
    ${tools_dir}/grib_compare -H -b totalLength $infile $tempGribA
done

# Second order packing does not apply to constant fields.
# So the result should use simple packing
${tools_dir}/grib_set -r -s typeOfLevel=surface,packingType=grid_second_order $data_dir/sample.grib2 $tempGribA
grib_check_key_equals $tempGribA packingType,const 'grid_second_order 0'
grib_check_key_equals $tempGribA isMessageValid 1

$EXEC ${test_dir}/grib_clone_headers_only $tempGribA $tempGribB
grib_check_key_equals $tempGribB packingType,const 'grid_simple 1'


# Clean up
rm -f $tempGribA $tempGribB

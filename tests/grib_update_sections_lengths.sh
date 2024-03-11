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

label="grib_update_sections_lengths_test"
tempGrib=temp.$label.grib

gfiles="$data_dir/sample.grib2 $data_dir/missing_field.grib1"
for f in $gfiles; do
    $EXEC ${test_dir}/grib_update_sections_lengths $f $tempGrib
    ${tools_dir}/grib_compare $f $tempGrib
done

rm -f $tempGrib

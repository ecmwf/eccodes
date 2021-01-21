#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
REDIRECT=/dev/null
label="grib_packing_order"
temp=${label}".grib.tmp"
temp_simple1=${label}".simple1.tmp"
temp_simple2=${label}".simple2.tmp"
temp_ccsds1=${label}".ccsds1.tmp"
temp_ccsds2=${label}".ccsds2.tmp"
temp_jpeg1=${label}".jpeg1.tmp"
temp_jpeg2=${label}".jpeg2.tmp"

# Simple Packing
$EXEC ${test_dir}/grib_packing_order grid_simple packing_type_before_values $temp_simple1
$EXEC ${test_dir}/grib_packing_order grid_simple values_before_packing_type $temp_simple2
grib_check_key_equals $temp_simple1 packingType grid_simple
${tools_dir}/grib_compare $temp_simple1 $temp_simple2

if [ $HAVE_AEC -eq 1 ]; then
    # CCSDS (AEC) Packing
    $EXEC ${test_dir}/grib_packing_order grid_ccsds packing_type_before_values $temp_ccsds1
    $EXEC ${test_dir}/grib_packing_order grid_ccsds values_before_packing_type $temp_ccsds2
    grib_check_key_equals $temp_ccsds1 packingType grid_ccsds
    ${tools_dir}/grib_compare $temp_ccsds1 $temp_ccsds2

    ${tools_dir}/grib_compare -c data:n $temp_simple1 $temp_ccsds1
fi

if [ $HAVE_JPEG -eq 1 ]; then
    # JPEG Packing
    $EXEC ${test_dir}/grib_packing_order grid_jpeg packing_type_before_values $temp_jpeg1
    $EXEC ${test_dir}/grib_packing_order grid_jpeg values_before_packing_type $temp_jpeg2
    grib_check_key_equals $temp_jpeg1 packingType grid_jpeg
    ${tools_dir}/grib_compare $temp_jpeg1 $temp_jpeg2

    ${tools_dir}/grib_compare -c data:n $temp_simple1 $temp_jpeg1
fi


# Clean up
rm -f $temp_simple1 $temp_simple2 $temp_ccsds1 $temp_ccsds2 $temp_jpeg1 $temp_jpeg2

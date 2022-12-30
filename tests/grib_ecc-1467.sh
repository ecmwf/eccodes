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

gfiles="
    sst_globus0083.grib
    constant_field.grib1
    constant_field.grib2
    reduced_latlon_surface.grib1
    reduced_latlon_surface.grib2
    regular_latlon_surface.grib1
    regular_latlon_surface.grib2
"
if [ $HAVE_AEC -eq 1 ]; then
    echo "Adding extra files (HAVE_AEC=1)"
    gfiles="ccsds.grib2 "$gfiles
fi

for f in $gfiles; do
    input=${data_dir}/$f
    ${tools_dir}/grib_ls -p numberOfDataPoints,numberOfCodedValues,numberOfMissing,avg $input
    $EXEC ${test_dir}/grib_ecc-1467 double $input
    $EXEC ${test_dir}/grib_ecc-1467 float  $input
done

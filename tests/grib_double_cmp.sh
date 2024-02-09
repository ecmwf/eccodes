#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

# --- test on the grid_ieee type of packing
# ---   check if retrieving data through grib_get_double_array and
# ---   grib_get_double_element provides the same result

. ./include.ctest.sh

label="grib_double_cmp_test"
tempGrib=temp.$label.grib

infiles="
    grid_ieee.grib
    regular_latlon_surface_constant.grib2
    gfs.complex.mvmu.grib2
    constant_field.grib2
    missing.grib2"

if [ $HAVE_JPEG -eq 1 ]; then
    infiles="jpeg.grib2 "$infiles
fi

if [ $HAVE_PNG -eq 1 ]; then
    infile=$data_dir/reduced_gaussian_model_level.grib2
    ${tools_dir}/grib_set -r -s packingType=grid_png $infile $data_dir/$tempGrib
    infiles="$tempGrib "$infiles
fi

if [ $HAVE_AEC -eq 1 ]; then
    infiles="ccsds.grib2 "$infiles
fi

for f in $infiles; do
    infile=$data_dir/$f
    $EXEC ${test_dir}/grib_double_cmp ${infile}
done

rm -f $data_dir/$tempGrib

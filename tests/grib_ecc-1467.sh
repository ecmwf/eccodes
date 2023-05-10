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

# Constant fields
gfiles="constant_field.grib1" # grid_simple, edition=1
gfiles="$gfiles constant_field.grib2" # grid_simple, edition=2

# Grid simple
gfiles="$gfiles sst_globus0083.grib" # grid_simple, edition=1
gfiles="$gfiles reduced_latlon_surface.grib1" # grid_simple, edition=1
gfiles="$gfiles reduced_latlon_surface.grib2" # grid_simple, edition=2
gfiles="$gfiles regular_latlon_surface.grib1" # grid_simple, edition=1
gfiles="$gfiles regular_latlon_surface.grib2" # grid_simple, edition=2

# Spectral complex
gfiles="$gfiles spherical_pressure_level.grib1" # spectral_complex, edition=1
gfiles="$gfiles spherical_pressure_level.grib2" # spectral_complex, edition=2

# Grid complex
#gfiles="$gfiles " # grid_complex, edition=1
gfiles="$gfiles gfs.complex.mvmu.grib2" # grid_complex, edition=2, g22order_packing

# Second order
gfiles="$gfiles lfpw.grib1 gen.grib gen_ext.grib second_ord_rbr.grib1" # grid_second_order, edition=1
#gfiles="$gfiles " # grid_second_order, edition=2

# CCSDS
if [ $HAVE_AEC -eq 1 ]; then
    gfiles="$gfiles ccsds.grib2"
fi

for f in $gfiles; do
    input=${data_dir}/$f
    # ${tools_dir}/grib_ls -w count=1 -p numberOfDataPoints,numberOfCodedValues,numberOfMissing,avg $input
    $EXEC ${test_dir}/grib_ecc-1467 $input
done

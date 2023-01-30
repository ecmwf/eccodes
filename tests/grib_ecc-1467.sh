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

# Constant Fields
gfiles+="constant_field.grib1 " # grid_simple, edition=1
gfiles+="constant_field.grib2 " # grid_simple, edition=2

# Grid Simple
gfiles+="sst_globus0083.grib " # grid_simple, edition=1
gfiles+="reduced_latlon_surface.grib1 " # grid_simple, edition=1
gfiles+="reduced_latlon_surface.grib2 " # grid_simple, edition=2
gfiles+="regular_latlon_surface.grib1 " # grid_simple, edition=1
gfiles+="regular_latlon_surface.grib2 " # grid_simple, edition=2

# Spherical Complex
gfiles+="spherical_pressure_level.grib1 " # spectral_complex, edition=1
gfiles+="spherical_pressure_level.grib2 " # spectral_complex, edition=2

# Grid Complex
gfiles+=" " # grid_complex, edition=1
gfiles+="gfs.complex.mvmu.grib2 " # grid_complex, edition=2, g22order_packing

# Second Order
#gfiles+="lfpw.grib1 " # grid_second_order, edition=1
#gfiles+=" " # grid_second_order, edition=2

# CCSDS
if [ $HAVE_AEC -eq 1 ]; then
    echo "Adding extra files (HAVE_AEC=1)"
    gfiles+="ccsds.grib2 "
fi

for f in $gfiles; do
    input=${data_dir}/$f
    ${tools_dir}/grib_ls -w count=1 -p numberOfDataPoints,numberOfCodedValues,numberOfMissing,avg $input
    $EXEC ${test_dir}/grib_ecc-1467 $input
done




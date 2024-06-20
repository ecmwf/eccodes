#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label="grib_grid_lamb_az_eq_area_test"

input=${data_dir}/regular_gaussian_pressure_level_constant.grib2
tempRef=grib_lamb_az_eq_area.ref

# Temporary files created for this test
tempFilt=temp.$label.filter
tempGrib=temp.$label.grib2
tempOut=temp.$label.txt
rm -f $tempFilt $tempGrib $tempOut

# --------------------
# Spherical Earth
# --------------------

# Create a filter
cat > $tempFilt<<EOF
set gridType = "lambert_azimuthal_equal_area";
set Nx = 10;
set Ny = 10;
set values = {2};
set numberOfDataPoints = 100;

set shapeOfTheEarth = 1; # Earth assumed spherical with radius specified (in m) by data producer 
set scaleFactorOfRadiusOfSphericalEarth = 0;
set scaledValueOfRadiusOfSphericalEarth = 6378388;

set numberOfValues = 100;
set latitudeOfFirstGridPointInDegrees = 67.575;
set longitudeOfFirstGridPointInDegrees = 326.5056;
set Dx = 5000000;
set Dy = 5000000;
set standardParallel = 48000000;
set centralLongitude = 9000000;
write;
EOF

# Use this filter and the input GRIB to create a new GRIB
rm -f "$tempGrib"
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input

# Now run the Geoiterator on the newly created GRIB file
${tools_dir}/grib_get_data $tempGrib > $tempOut

# Compare output with reference. If the diff fails, script will immediately exit with status 1
diff $tempRef $tempOut

grib_check_key_equals $tempGrib standardParallelInDegrees,centralLongitudeInDegrees '48 9'
grib_check_key_equals $tempGrib xDirectionGridLengthInMetres,yDirectionGridLengthInMetres '5000 5000'

# Nearest
${tools_dir}/grib_ls -l 67,-33,1 $tempGrib

# jPointsAreConsecutive
tempOutA=temp.$label.A.txt
tempOutB=temp.$label.B.txt
${tools_dir}/grib_get_data -s jPointsAreConsecutive=0 $tempGrib > $tempOutA
${tools_dir}/grib_get_data -s jPointsAreConsecutive=1 $tempGrib > $tempOutB
# Results should be different.
set +e
diff $tempOutA $tempOutB > /dev/null
status=$?
set -e
[ $status -ne 0 ]
rm -f $tempOutA $tempOutB


# --------------------
# Oblate spheroid
# --------------------

cat > $tempFilt<<EOF
set edition = 2;
set gridType = "lambert_azimuthal_equal_area";
set Nx = 10;
set Ny = 10;
set values = {2};
set numberOfDataPoints = 100;

set shapeOfTheEarth = 4; # Earth assumed oblate spheroid

set numberOfValues = 100;
set latitudeOfFirstGridPointInDegrees = 67.575;
set longitudeOfFirstGridPointInDegrees = 326.5056;
set Dx = 5000000;
set Dy = 5000000;
set standardParallel = 48000000;
set centralLongitude = 9000000;
write;
EOF

# Use this filter and the input GRIB to create a new GRIB
rm -f "$tempGrib"
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input

${tools_dir}/grib_get_data $tempGrib > $tempOut

# ECC-1818
# ---------
cat > $tempFilt<<EOF
set gridType = "lambert_azimuthal_equal_area";
set shapeOfTheEarth = 5;
set numberOfPointsAlongXAxis = 432;
set numberOfPointsAlongYAxis = 432;
set latitudeOfFirstGridPoint = 16623926;
set longitudeOfFirstGridPoint = -135000000;
set standardParallelInMicrodegrees = 90000000;
set centralLongitudeInMicrodegrees = 0;
set xDirectionGridLengthInMillimetres = 25000000;
set yDirectionGridLengthInMillimetres = 25000000;
set values = 666;
set numberOfDataPoints = 186624;
set numberOfValues = 186624;
write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_get_data $tempGrib > $tempOut


# Clean up
rm -f $tempFilt $tempOut $tempGrib

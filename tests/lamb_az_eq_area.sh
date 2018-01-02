#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh
#set -x

GRIB_INFILE=${data_dir}/regular_gaussian_pressure_level_constant.grib2
REF_FILE=lamb_az_eq_area.ref

# Temporary files created for this test
FILTER_FILE=lamb_az_eq_area.filter
GRIB_OUTFILE=lamb_az_eq_area.grib2
DATA_OUTFILE=lamb_data.txt
rm -f $FILTER_FILE $GRIB_OUTFILE $DATA_OUTFILE

# Create a filter
cat > $FILTER_FILE<<EOF
set edition=2;
set gridType="lambert_azimuthal_equal_area";
set Nx = 10;
set Ny = 10;
set values={2};
set numberOfDataPoints=100;
set shapeOfTheEarth=1;
set scaleFactorOfRadiusOfSphericalEarth=0;
set scaledValueOfRadiusOfSphericalEarth=6378388;
set numberOfValues=100;
set latitudeOfFirstGridPointInDegrees = 67.575;
set longitudeOfFirstGridPointInDegrees = -33.4944;
set Dx = 5000000;
set Dy = 5000000;
set standardParallel = 48000000;
set centralLongitude = 9000000;
write "lamb_az_eq_area.grib2";
EOF

# Use this filter and the input GRIB to create a new GRIB
${tools_dir}/grib_filter $FILTER_FILE $GRIB_INFILE
if [ ! -f "$GRIB_OUTFILE" ]; then
   echo Failed to create output GRIB from filter >&2
   exit 1
fi
# Now get the data from the newly created GRIB file
${tools_dir}/grib_get_data $GRIB_OUTFILE > $DATA_OUTFILE

# Compare output with reference. If the diff fails, script will immediately exit with status 1
diff $DATA_OUTFILE $REF_FILE

# Clean up
rm -f $FILTER_FILE $GRIB_OUTFILE $DATA_OUTFILE


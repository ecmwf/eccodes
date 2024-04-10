#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

# Define a common label for all the tmp files
label="grib_lambert_conformal_test"
tempFilt="temp.${label}.filt"
tempGrib="temp.${label}.grib"
tempOut="temp.${label}.out"

input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Create a filter
cat > $tempFilt <<EOF
 set gridType="lambert";
 set numberOfDataPoints=294000;
 set shapeOfTheEarth=6;
 set Nx=588;
 set Ny=500;
 set latitudeOfFirstGridPoint=40442000;
 set longitudeOfFirstGridPoint=353559000;
 set LaD=60000000;
 set LoV=2200000;
 set Dx=2499000;
 set Dy=2499000;
 set Latin1=46401000;
 set Latin2=46401000;
 set numberOfValues=294000;
 write;
EOF

# Use filter on input to create a new Lambert conformal GRIB
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input
if [ ! -f "$tempGrib" ]; then
   echo 'Failed to create output GRIB from filter' >&2
   exit 1
fi
# Invoke Geoiterator on the newly created GRIB file
${tools_dir}/grib_get_data $tempGrib > $tempOut

${tools_dir}/grib_ls -l 50,0 $tempGrib

# Oblate earth
# --------------
cat > $tempFilt <<EOF
 set gridType="lambert";
 set numberOfDataPoints=294000;
 set Nx=588;
 set Ny=500;
 set latitudeOfFirstGridPoint=40442000;
 set longitudeOfFirstGridPoint=353559000;
 set LaD=60000000;
 set LoV=2200000;
 set Dx=2499000;
 set Dy=2499000;
 set Latin1=46401000;
 set Latin2=46401000;
 set shapeOfTheEarth=2;  # oblate earth
 set numberOfValues=294000;
 write;
EOF

# Use this filter and the input GRIB to create a new GRIB
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input
if [ ! -f "$tempGrib" ]; then
   echo 'Failed to create output GRIB from filter' >&2
   exit 1
fi
grib_check_key_equals $tempGrib 'earthIsOblate,earthMinorAxisInMetres,earthMajorAxisInMetres' '1 6356775 6378160'

# Invoke Geoiterator on the oblate lambert GRIB
${tools_dir}/grib_get_data $tempGrib > $tempOut

# Nearest neighbour on the oblate lambert GRIB
${tools_dir}/grib_ls -l 40.44,353.56 $tempGrib


# Clean up
rm -f $tempFilt $tempGrib $tempOut

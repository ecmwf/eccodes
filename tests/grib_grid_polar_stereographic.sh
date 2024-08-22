#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

if [ $HAVE_GEOGRAPHY -eq 0 ]; then
    exit 0
fi

# Define a common label for all the tmp files
label="grib_polar_stereographic_test"
tempFilt="temp.${label}.filt"
tempGrib="temp.${label}.grib"
tempOut="temp.${label}.out"

input=$ECCODES_SAMPLES_PATH/polar_stereographic_pl_grib2.tmpl

# Invoke Geoiterator
${tools_dir}/grib_get_data $input > $tempOut
# Nearest
${tools_dir}/grib_ls -l 60,0 $input

# Scanning mode
${tools_dir}/grib_get_data -s iScansNegatively=1 $input > $tempOut

# Failing case
# -------------
${tools_dir}/grib_set -s shapeOfTheEarth=2 $input $tempGrib  # oblate earth
set +e
${tools_dir}/grib_get_data $tempGrib > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Only supported for spherical earth" $tempOut

# Bad geography
set +e
${tools_dir}/grib_get_data -s Nx=1 $input > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Wrong number of points" $tempOut


# Clean up
rm -f $tempFilt $tempGrib $tempOut

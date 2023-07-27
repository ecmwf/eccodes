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

label="grid_ecc-530_test"
tempGrib=temp.$label.grib
tempFilt=temp.${label}.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat > $tempFilt <<EOF
 set tablesVersion = 30;
 set typeOfFirstFixedSurface = 160;
 set scaleFactorOfFirstFixedSurface = 3;
 set scaledValueOfFirstFixedSurface = 200;

 set scaleFactorOfSecondFixedSurface = 4;
 set scaledValueOfSecondFixedSurface = 300;
 set typeOfSecondFixedSurface = 165; # intentionally put at the end
 write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib scaleFactorOfFirstFixedSurface  3
grib_check_key_equals $tempGrib scaledValueOfFirstFixedSurface  200
grib_check_key_equals $tempGrib scaleFactorOfSecondFixedSurface 4
grib_check_key_equals $tempGrib scaledValueOfSecondFixedSurface 300

# Clean up
rm -f $tempFilt $tempGrib

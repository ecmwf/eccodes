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

# ---------------------------------------------------------
# Update shapeOfTheEarth and radius values for reduced_gg gridType
# ECC-2339
# ---------------------------------------------------------

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

r1=$( ${tools_dir}/grib_get -s gridType=reduced_gg,shapeOfTheEarth=6 -p radius $sample_grib2 )
r2=$( ${tools_dir}/grib_get -s gridType=reduced_gg,shapeOfTheEarth=0 -p radius $sample_grib2 )

if [ "$r1" == "$r2" ]; then
    echo "Test failed: radius values are equal for different shapeOfTheEarth values"
    echo "radius with shapeOfTheEarth=6: $r1"
    echo "radius with shapeOfTheEarth=0: $r2"
    exit 1
fi

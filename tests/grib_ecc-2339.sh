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
# This is the test for JIRA issue ECC-2339
# The fixed radius/axes of the earth must follow shapeOfTheEarth.
# When a section is rebuilt (e.g. by a gridType change), the constant
# of the previous shape must not be copied over that of the new one.
# ---------------------------------------------------------

label="grib_ecc-2339_test"
tempGrib=temp.$label.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# The sample has shapeOfTheEarth=0 (radius 6367470)
grib_check_key_equals $sample_grib2 shapeOfTheEarth,radius:i '0 6367470'

# Check the keys after setting some keys in memory, without writing a file
# Arguments: keys to set, keys to get, expected result
check_in_memory()
{
    a_set=$1
    a_keys=$2
    a_expected=$3
    a_result=$( ${tools_dir}/grib_get -F "%.3f" -s $a_set -p $a_keys $sample_grib2 )
    if [ "$a_result" != "$a_expected" ]; then
        echo "Set:      '$a_set'"
        echo "Key(s):   '$a_keys'"
        echo "Expected: '$a_expected'"
        echo "Result:   '$a_result'"
        exit 1
    fi
}

# Spherical earth: shapeOfTheEarth -> radius
for shape_radius in "0 6367470" "6 6371229" "8 6371200"; do
    set -- $shape_radius
    shape=$1
    radius=$2

    # Written to a file and read back
    ${tools_dir}/grib_set -s shapeOfTheEarth=$shape $sample_grib2 $tempGrib
    grib_check_key_equals $tempGrib radius:i $radius

    # Changing only the shape
    check_in_memory shapeOfTheEarth=$shape radius:i $radius

    # Changing the shape together with the grid type, in either order.
    # gridType rebuilds section 3, which must not keep the radius of the old shape.
    check_in_memory gridType=reduced_gg,shapeOfTheEarth=$shape radius:i $radius
    check_in_memory shapeOfTheEarth=$shape,gridType=reduced_gg radius:i $radius
done

# Oblate spheroid earth: same problem with the major/minor axes (WGS84)
check_in_memory gridType=reduced_gg,shapeOfTheEarth=5 earthMajorAxis,earthMinorAxis '6378137 6356752.314'

# A radius explicitly set by the user is kept
check_in_memory gridType=reduced_gg,shapeOfTheEarth=1,radius=6400000 radius:i 6400000

# Clean up
rm -f $tempGrib

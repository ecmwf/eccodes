#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-979.
# Directly setting keys that use the accessor
#   from_scale_factor_scaled_value
# ---------------------------------------------------------
label="grib_ecc-979-test"
temp=temp.${label}
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s shapeOfTheEarth=1,radius=6371229   $sample2 $temp
grib_check_key_equals $temp scaledValueOfRadiusOfSphericalEarth,scaleFactorOfRadiusOfSphericalEarth '6371229 0'
${tools_dir}/grib_set -s shapeOfTheEarth=1,radius=6371229.0 $sample2 $temp
grib_check_key_equals $temp scaledValueOfRadiusOfSphericalEarth,scaleFactorOfRadiusOfSphericalEarth '6371229 0'

# Oblate
${tools_dir}/grib_set -s shapeOfTheEarth=7,earthMajorAxis=6378137.0,earthMinorAxis=6356752.3142 $sample2 $temp
grib_check_key_equals $temp scaleFactorOfEarthMajorAxis,scaledValueOfEarthMajorAxis '0 6378137'
grib_check_key_equals $temp scaleFactorOfEarthMinorAxis,scaledValueOfEarthMinorAxis '2 635675231'


# Test edge case
${tools_dir}/grib_set -s shapeOfTheEarth=1,radius=0 $sample2 $temp
grib_check_key_equals $temp scaledValueOfRadiusOfSphericalEarth,scaleFactorOfRadiusOfSphericalEarth '0 0'

# Check larger factor
${tools_dir}/grib_set -s shapeOfTheEarth=1,radius=678.1234 $sample2 $temp
grib_check_key_equals $temp scaledValueOfRadiusOfSphericalEarth,scaleFactorOfRadiusOfSphericalEarth '6781234 4'


# Clean up
rm -f $temp

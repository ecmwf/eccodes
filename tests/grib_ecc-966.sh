#!/bin/sh
# (C) Copyright 2005- ECMWF.
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
# This is the test for the JIRA issue ECC-966.
# Crash: setting shapeOfTheEarth=1 but scale factor missing
# ---------------------------------------------------------
label="grib_ecc-966-test"
tempGrib=temp.${label}.grib
tempErrs=temp.${label}.errs

in=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s shapeOfTheEarth=1,scaledValueOfRadiusOfSphericalEarth=6367999 $in $tempGrib
grib_check_key_equals $tempGrib scaleFactorOfRadiusOfSphericalEarth MISSING
grib_check_key_equals $tempGrib 'radius:i' 6367999 2>$tempErrs
grep -q "ECCODES ERROR   :  unpack_double for radius" $tempErrs

# Clean up
rm -f $tempGrib $tempErrs

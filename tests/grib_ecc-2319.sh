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
# This is the test for JIRA issue ECC-2319
# The numberOfDataPoints and numberOfValues keys are not set correctly when using grib_set -s gridSpec=... on spectral data.
# ---------------------------------------------------------

REDIRECT=/dev/null

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib=temp.$label.grib
sample_grib2=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl

if [ $HAVE_ECKIT_GEO -ne 1 ]; then
    echo "$0: This test is disabled when HAVE_ECKIT_GEO=OFF"
    exit 0
fi

# Check env. variable too
if [ "${ECCODES_ECKIT_GEO:-0}" -eq 0 ]
then
    echo "$0: This test is disabled (env. variable ECCODES_ECKIT_GEO=0)"
    exit 0
fi
set -u

$tools_dir/grib_set -s gridSpec="{"grid":"T106"}" $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib numberOfDataPoints,numberOfValues "11556 11556"

# Clean up
rm -f $tempGrib

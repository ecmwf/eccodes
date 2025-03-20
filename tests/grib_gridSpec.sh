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

if [ $HAVE_GEOGRAPHY -ne 1 ]; then
    echo "$0: This test is disabled when HAVE_GEOGRAPHY=OFF"
    exit 0
fi

label="grib_gridSpec_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempText=temp.$label.txt

# Key is not there for spectral data (only gridded)
infile=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl
result=$( ${tools_dir}/grib_get -fp gridSpec $infile )
[ "$result" = "not_found" ]

if [ $HAVE_ECKIT_GEO -ne 1 ]; then
    echo "$0: This test is disabled when HAVE_ECKIT_GEO=OFF"
    exit 0
fi

# Gridded data
infile=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_get -p gridSpec $infile
grib_check_key_equals $infile gridSpec '{"east":30,"grid":[2,2],"north":60,"shape":[16,31],"south":0,"west":0}'

infile=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
${tools_dir}/grib_get -p gridSpec $infile
grib_check_key_equals $infile gridSpec '{"grid":"N48"}'


# Clean up
rm -f $tempGrib $tempFilt $tempText

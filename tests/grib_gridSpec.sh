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

# # Key is not there for spectral data (only gridded)
# infile=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl
# result=$( ${tools_dir}/grib_get -fp gridSpec $infile )
# [ "$result" = "not_found" ]

if [ $HAVE_ECKIT_GEO -ne 1 ]; then
    echo "$0: This test is disabled when HAVE_ECKIT_GEO=OFF"
    exit 0
fi

# Check env. variable too
set +u
if test "x$ECCODES_ECKIT_GEO" = "x"; then
    echo "$0: This test is disabled (env. variable ECCODES_ECKIT_GEO is not set)"
    exit 0
fi
set -u

# Decode gridSpec
# ----------------
infile=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_get -p gridSpec $infile
grib_check_key_equals $infile gridSpec '{"east":30,"grid":[2,2],"north":60,"south":0,"west":0}'

infile=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
${tools_dir}/grib_get -p gridSpec $infile
grib_check_key_equals $infile gridSpec '{"grid":"N48"}'

infile=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
${tools_dir}/grib_set -s gridSpec='{grid:N32}' $infile $tempGrib
# ${tools_dir}/grib_ls -jn geography $infile $tempGrib
grib_check_key_equals $tempGrib N,Nj '32 64'
# ${tools_dir}/grib_compare $infile $tempGrib


# Encode gridSpec
# ----------------
infile=${data_dir}/missing.grib2
cat >$tempFilt<<EOF
    # Decode the current gridSpec
    transient gs_from_msg1 = gridSpec;

    # Feed it to the encoder
    set gridSpec=gs_from_msg1;
    # Decode it again
    transient gs_from_msg2 = gridSpec;

    print "[gs_from_msg1=]";
    print "[gs_from_msg2=]";
    # The two should be the same
    assert( gs_from_msg1 is gs_from_msg2 );
    write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $infile
${tools_dir}/grib_compare $infile $tempGrib
rm -f $tempGrib

# Can encode and decode in one step!
infile=${data_dir}/missing.grib2
cat >$tempFilt<<EOF
    set gridSpec = gridSpec;
    write;
EOF
ECCODES_DEBUG=-1 ${tools_dir}/grib_filter -o $tempGrib $tempFilt $infile > $tempText 2>&1
${tools_dir}/grib_compare $infile $tempGrib
grep -q "ECCODES DEBUG grib_set_from_grid_spec: grib_set_values, setting 16 key/value pairs" $tempText


# Error conditions
# ----------------
infile=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
set +e
${tools_dir}/grib_set -s gridSpec='{grid:H36}' $infile $tempGrib 2>$tempText
status=$?
set -e
[ $status -ne 0 ]
grep -q "'healpix' specified but input is GRIB edition 1" $tempText


# Clean up
rm -f $tempGrib $tempFilt $tempText

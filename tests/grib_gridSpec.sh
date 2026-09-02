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
if [ "${ECCODES_ECKIT_GEO:-0}" -eq 0 ]
then
    echo "$0: This test is disabled (env. variable ECCODES_ECKIT_GEO=0)"
    exit 0
fi
set -u

# Decode gridSpec
# ----------------
infile=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_get -p gridSpec $infile
grib_check_key_equals $infile gridSpec '{"area":[60,0,0,30],"grid":[2,2]}'

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


# ECC-2318: a degenerate grid (a single point in a direction) is rejected
# -----------------------------------------------------------------------
# Such a grid has a zero increment, which a gridSpec cannot describe ('grid' would hold a 0),
# so it is an error in both directions rather than a spec that does not describe the grid.
# The sample is a regular_ll grid, area = [60,0,0,30], increments = 2/2 degrees
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Decoding: a single row (Nj=1), a single column (Ni=1) and a single point (Ni=Nj=1)
for setKeysResult in \
    'Ni=16,Nj=1,numberOfDataPoints=16,latitudeOfLastGridPointInDegrees=60|{"area":[60,0,60,30],"grid":[2,0]}' \
    'Ni=1,Nj=31,numberOfDataPoints=31,longitudeOfLastGridPointInDegrees=0|{"area":[60,0,0,0],"grid":[0,2]}' \
    'Ni=1,Nj=1,numberOfDataPoints=1,latitudeOfLastGridPointInDegrees=60,longitudeOfLastGridPointInDegrees=0|{"area":[60,0,60,0],"grid":[0,0]}'
do
    setKeys=$(echo $setKeysResult | cut -d'|' -f1)
    expectedSpec=$(echo $setKeysResult | cut -d'|' -f2)
    set +e
    ${tools_dir}/grib_set -s $setKeys $sample $tempGrib
    ${tools_dir}/grib_get -p gridSpec $tempGrib > $tempText
    status=$?
    set -e
    [ $status -eq 0 ]
    cat $tempText
    grep -F -q "$expectedSpec" $tempText
done
rm -f $tempGrib

# Encoding: an explicitly degenerate spec, and an area with no extent
for spec in \
    '{"area":[0,0,0,0],"grid":[0,0]}|{"area":[0,0,0,0],"grid":[0,0]}' \
    '{"area":[0,0,0,0],"grid":[1,1]}|{"area":[0,0,0,0],"grid":[0,0]}' \
    '{"area":[60,0,60,30],"grid":[2,2]}|{"area":[60,0,60,30],"grid":[2,0]}' \
    '{"area":[60,0,0,0],"grid":[2,2]}|{"area":[60,0,0,0],"grid":[0,2]}'
do
    setKeys=$(echo $spec | cut -d'|' -f1)
    expectedSpec=$(echo $spec | cut -d'|' -f2)
    set +e
    ${tools_dir}/grib_set -s gridSpec="$setKeys" $sample $tempGrib
    ${tools_dir}/grib_get -p gridSpec $tempGrib > $tempText
    status=$?
    set -e
    [ $status -eq 0 ]
    grep -F -q $expectedSpec $tempText
done
rm -f $tempGrib

# Grids that do not carry increments in their spec are unaffected
grib_check_key_equals $ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl gridSpec '{"grid":"N48"}'
grib_check_key_equals $sample gridSpec '{"area":[60,0,0,30],"grid":[2,2]}'


# Clean up
rm -f $tempGrib $tempFilt $tempText

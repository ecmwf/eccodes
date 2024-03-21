#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

# Define a common label for all the tmp files
label="grib_healpix_test"
tempFilt="temp.${label}.filt"
tempGrib="temp.${label}.grib"
tempLog="temp.${label}.log"

input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

latest=`${tools_dir}/grib_get -p tablesVersionLatest $input`

# Check basic keys and grid template
# -----------------------------------
cat > $tempFilt <<EOF
  set tablesVersion = $latest;
  set gridType = "healpix";
  set longitudeOfFirstGridPointInDegrees = 45;
  set numberOfPointsAlongASide = 32;
  write;
EOF

cat $tempFilt
# Use filter on input to create a new HEALPix GRIB
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input
if [ ! -f "$tempGrib" ]; then
   echo 'Failed to create output GRIB from filter' >&2
   exit 1
fi
grib_check_key_equals $tempGrib gridType,orderingConvention,N,Nside 'healpix ring 32 32'
grib_check_key_equals $tempGrib gridDefinitionTemplateNumber,gridName '150 H32'
if [ $latest -gt 31 ]; then
  grib_check_key_equals $tempGrib gridDefinitionDescription 'Hierarchical Equal Area isoLatitude Pixelization grid'
fi

${tools_dir}/grib_dump -O -p section_3 $tempGrib
${tools_dir}/grib_ls -jn geography $tempGrib > $tempLog
grep -q "orderingConvention.*ring" $tempLog

# Geoiterator ring-ordering
# --------------------------
rm -f $tempGrib
cat > $tempFilt <<EOF
  set tablesVersion = $latest;
  set gridType = "healpix";
  set longitudeOfFirstGridPointInDegrees = 45;
  set numberOfPointsAlongASide = 1;
  set values = {1,2,3,4,5,6,7,8,9,10,11,12}; # count=12*N*N
  write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input
${tools_dir}/grib_get_data $tempGrib
val=$(${tools_dir}/grib_get -l 0,0,1 $tempGrib | tr -d ' ')
[ "$val" = 5 ]

# Check other iterator-related keys
cat > $tempFilt <<EOF
 print "latLonValues=[latLonValues]";
 print "latitudes=[latitudes]";
 print "longitudes=[longitudes]";
 print "distinctLatitudes=[distinctLatitudes]";
 print "distinctLongitudes=[distinctLongitudes]";
EOF

${tools_dir}/grib_filter $tempFilt $tempGrib


# Geoiterator Nested ordering
# ---------------------------
rm -f $tempGrib
cat > $tempFilt <<EOF
  set tablesVersion = $latest;
  set gridType = "healpix";
  set longitudeOfFirstGridPointInDegrees = 45;
  set numberOfPointsAlongASide = 2;
  set orderingConvention = "nested";
  set values = { 0,1,2,3,4,5,6,7,
    8,9,10,11,12,13,14,15,
    16,17,18,19,20,21,22,23,
    24,25,26,27,28,29,30,31,
    32,33,34,35,36,37,38,39,
    40,41,42,43,44,45,46,47};
  # count of values = 48 = 12 * N * N
  write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input
${tools_dir}/grib_get_data $tempGrib > $tempLog

# Nested: N must be a power of 2
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s gridType=healpix,Nside=3,orderingConvention=nested,numberOfDataPoints=108,numberOfValues=108 $input $tempGrib
set +e
${tools_dir}/grib_get_data $tempGrib > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Nside must be a power of 2" $tempLog


# Nested N=1
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s gridType=healpix,Nside=1,orderingConvention=nested,numberOfDataPoints=12,numberOfValues=12 $input $tempGrib
${tools_dir}/grib_get_data $tempGrib > $tempLog

# Invalid cases
# ------------------
# Bad ordering
${tools_dir}/grib_set -s gridType=healpix,Nside=1,ordering=6 $input $tempGrib
set +e
${tools_dir}/grib_get_data $tempGrib > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Only orderingConvention.*are supported" $tempLog

# N = 0
set +e
${tools_dir}/grib_get_data -sN=0 $tempGrib > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Nside must be greater than zero" $tempLog


# Clean up
rm -f $tempFilt $tempGrib $tempLog

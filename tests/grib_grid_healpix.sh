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
tempFilter="temp.${label}.filt"
tempGrib="temp.${label}.grib"
tempLog="temp.${label}.log"

input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

latest=`${tools_dir}/grib_get -p tablesVersionLatest $input`

# Create a filter
cat > $tempFilter <<EOF
  set tablesVersion = $latest;
  set gridType = "healpix";
  set longitudeOfFirstGridPointInDegrees = 45;
  set numberOfPointsAlongASide = 32;
  write;
EOF

cat $tempFilter
# Use filter on input to create a new HEALPix GRIB
${tools_dir}/grib_filter -o $tempGrib $tempFilter $input
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
${tools_dir}/grib_ls -jn geography $tempGrib

# Geoiterator
# -------------
rm -f $tempGrib
cat > $tempFilter <<EOF
  set tablesVersion = $latest;
  set gridType = "healpix";
  set longitudeOfFirstGridPointInDegrees = 45;
  set numberOfPointsAlongASide = 1;
  set values = {1,2,3,4,5,6,7,8,9,10,11,12}; # count=12*N*N
  write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilter $input
${tools_dir}/grib_get_data $tempGrib
val=$(${tools_dir}/grib_get -l 0,0,1 $tempGrib | tr -d ' ')
[ "$val" = 5 ]

# Check other iterator-related keys
cat > $tempFilter <<EOF
 print "latLonValues=[latLonValues]";
 print "latitudes=[latitudes]";
 print "longitudes=[longitudes]";
 print "distinctLatitudes=[distinctLatitudes]";
 print "distinctLongitudes=[distinctLongitudes]";
EOF

${tools_dir}/grib_filter $tempFilter $tempGrib

# Invalid cases
# --------------
set +e
${tools_dir}/grib_get_data -sN=0 $tempGrib > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Nside must be greater than zero" $tempLog

set +e
${tools_dir}/grib_get_data -s orderingConvention=nested $tempGrib > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Only ring ordering is supported" $tempLog


# Clean up
rm -f $tempFilter $tempGrib $tempLog

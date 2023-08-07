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
tempOut="temp.${label}.out"

input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Create a filter
cat > $tempFilter <<EOF
  set tablesVersion = 32;
  set gridType = "healpix";
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
grib_check_key_equals $tempGrib gridDefinitionDescription 'Hierarchical Equal Area isoLatitude Pixelization grid'

${tools_dir}/grib_dump -O -p section_3 $tempGrib
${tools_dir}/grib_ls -jn geography $tempGrib

# Geoiterator
# -------------
rm -f $tempGrib
cat > $tempFilter <<EOF
  set tablesVersion = 32;
  set gridType = "healpix";
  set numberOfPointsAlongASide = 1;
  set values = {1,2,3,4,5,6,7,8,9,10,11,12}; # 12*N*N
  write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilter $input
${tools_dir}/grib_get_data $tempGrib

# Clean up
rm -f $tempFilter $tempGrib $tempOut

#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

# Define a common label for all the tmp files
label="grib_healpix_test"
tempFilter="temp.${label}.filt"
tempGrib="temp.${label}.grib"
tempOut="temp.${label}.out"

input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Create a filter
cat > $tempFilter <<EOF
 #set centre = 98; # local ECMWF grid
 #set localTablesVersion = 1;

 # Note: setting the gridType will also set centre=98 and
 #       localTablesVersion=1
 set gridType = "healpix";
 set nside = 1200;

 write;
EOF

# Use filter on input to create a new HEALPix GRIB
${tools_dir}/grib_filter -o $tempGrib $tempFilter $input
if [ ! -f "$tempGrib" ]; then
   echo 'Failed to create output GRIB from filter' >&2
   exit 1
fi
grib_check_key_equals $tempGrib gridType,orderingConvention,N                          'healpix ring 1200'
grib_check_key_equals $tempGrib gridDefinitionTemplateNumber,gridDefinitionDescription '33000 HEALPix grid'

${tools_dir}/grib_dump -O -p section_3 $tempGrib
${tools_dir}/grib_ls -jn geography $tempGrib

# Clean up
rm -f $tempFilter $tempGrib $tempOut

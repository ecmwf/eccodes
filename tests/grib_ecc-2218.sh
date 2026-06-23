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
# This is the test for JIRA issue ECC-2218
# GRIB2: levtype hl for post-mtg2 encodings
# ---------------------------------------------------------

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# in ERA6, 100m heightAboveGround -> levtype hl
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class="e6";
set tablesVersion=35;
set paramId=131;
set typeOfFirstFixedSurface=103;
set level=100;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib levtype,paramId "hl 131"

# in ERA6, 10m heightAboveGround -> levtype sfc
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class="e6";
set tablesVersion=35;
set paramId=165;
set typeOfFirstFixedSurface=103;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib levtype,paramId "sfc 165"

# AIFS pre-mtg2 level=100 -> levtype=sfc
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class="ai";
set tablesVersion=35;
set paramId=228246;
set typeOfFirstFixedSurface=103;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib levtype,paramId "sfc 228246"

# od data post-mtg2 level=100 -> levtype=sfc
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class="od";
set tablesVersion=36;
set paramId=131;
set typeOfFirstFixedSurface=103;
set level=100;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib levtype,paramId "hl 131"

# Clean up
rm -f $tempGrib $tempFilt

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

label="grib_mars_keys2_test"
tempGrib=temp.${label}.grib

grib1_sample=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
grib2_sample=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

# Check lowercase/uppercase
${tools_dir}/grib_set -s class=YT,type=EM,stream=DCWV $grib1_sample $tempGrib
grib_check_key_equals $tempGrib mars.class,mars.type,mars.stream 'yt em dcwv'
grib_check_key_equals $tempGrib marsClass:i,marsType:i,marsStream:i '18 17 1029'


# Check all combinations
# ------------------------
i=0
for cfg in $ECCODES_DEFINITION_PATH/mars/grib.*.*.def; do
  b=$(basename $cfg)
  # Each file should be of the form grib.$stream.$type.def
  stream=$(echo $b | awk -F. '{print $2}')
  type=$(echo $b | awk -F. '{print $3}')
  if [ $type = "fc" -o $type = "an" ]; then
    ${tools_dir}/grib_set -s stream=$stream,type=$type $grib2_sample $tempGrib
    ${tools_dir}/grib_ls -m $tempGrib > /dev/null
    i=$((i + 1))
  fi
done
echo "Checked $i files"

# Clean up
rm -f $tempGrib

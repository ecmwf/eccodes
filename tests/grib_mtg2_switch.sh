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

REDIRECT=/dev/null

label="grib_mtg2_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Sample has tablesVersion < 33
grib_check_key_equals $sample_grib2 tablesVersion     4
grib_check_key_equals $sample_grib2 paramIdFilename   "paramId.lte33.def"
grib_check_key_equals $sample_grib2 shortNameFilename "shortName.lte33.def"

#...
#infile=${data_dir}/SOME_FILE
#cat >$tempFilt<<EOF
#EOF
#${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
#${tools_dir}/grib_get
${tools_dir}/grib_set -s tablesVersion=34 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib paramIdFilename   "paramId.def"
grib_check_key_equals $tempGrib shortNameFilename "shortName.def"


# Clean up
rm -f $tempGrib $tempFilt $tempLog $tempOut $tempRef

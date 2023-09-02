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

label="grib_ifsParam_test"
tempLog=temp.$label.log
tempGrib=temp.$label.grib
tempText=temp.$label.txt

sample_g1="$ECCODES_SAMPLES_PATH/GRIB1.tmpl"
sample_g2="$ECCODES_SAMPLES_PATH/GRIB2.tmpl"

${tools_dir}/grib_set -s paramId=167 $sample_g1 $tempGrib
grib_check_key_equals $tempGrib ifsParam 167

${tools_dir}/grib_set -s paramId=129001 $sample_g1 $tempGrib
grib_check_key_equals $tempGrib ifsParam 1

${tools_dir}/grib_set -s paramId=200001 $sample_g1 $tempGrib
grib_check_key_equals $tempGrib ifsParam 1

${tools_dir}/grib_set -s paramId=211001 $sample_g1 $tempGrib
grib_check_key_equals $tempGrib ifsParam 210001


# Clean up
rm -f $tempGrib $tempText $tempLog

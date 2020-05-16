#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1053
# It tests the human readable step key
# ---------------------------------------------------------
label="grib_ecc-1053-test"
temp=temp.${label}.grib
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl

${tools_dir}/grib_set -s unitOfTimeRange=254,P1=250 $sample_g1 $temp
grib_check_key_equals $temp stepHumanReadable '0h 4m 10s'

${tools_dir}/grib_set -s unitOfTimeRange=0,P1=200 $sample_g1 $temp
grib_check_key_equals $temp stepHumanReadable '3h 20m'

rm -f $temp

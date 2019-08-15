#!/bin/sh
# Copyright 2005-2019 ECMWF.
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
# This is the test for the JIRA issue ECC-967.
# GRIB2: Fallback if tables version > latest installed version
# ---------------------------------------------------------
label="grib_ecc-967-test"
temp=temp.${label}.grib

# Pick a master tables version number way to big!
# But one day we will actually get to it.
input=$ECCODES_SAMPLES_PATH/reduced_gg_pl_160_grib2.tmpl
${tools_dir}/grib_set -s tablesVersion=253 $input $temp
grib_check_key_equals $temp 'typeOfFirstFixedSurface:i' '100'
grib_check_key_equals $temp 'typeOfFirstFixedSurface:s' 'pl'
grib_check_key_equals $temp 'mars.levtype' 'pl'

# Clean up
rm -f $temp

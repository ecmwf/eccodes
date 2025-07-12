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

label="grib_datasetForLocal_test"
tempGrib=temp.$label.grib

# Sample with mars local section
sample=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl

test_ds()
{
    _class=$1
    _result=$2
    ${tools_dir}/grib_set -s class=$_class $sample $tempGrib
    grib_check_key_equals $tempGrib datasetForLocal $_result
}

for c in ce ul; do
    ${tools_dir}/grib_set -s class=$c $sample $tempGrib
    grib_check_key_equals $tempGrib datasetForLocal hydro
done

for c in e2 e4 ea ei em ep er et l5; do
    ${tools_dir}/grib_set -s class=$c $sample $tempGrib
    grib_check_key_equals $tempGrib datasetForLocal era
done

test_ds ci cerise
test_ds e6 era6
test_ds gw fire
test_ds ng nextgems


# Clean up
rm -f $tempGrib

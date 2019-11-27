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
# This is the test for data quality checks
# ---------------------------------------------------------
label="grib_data_quality"
tempOut=temp.${label}.out
tempErr=temp.${label}.err

input1=${data_dir}/reduced_gaussian_surface.grib1
input2=${data_dir}/reduced_gaussian_surface.grib2

# Data quality checks disabled. Create cause huge values for temperature
unset ECCODES_GRIB_DATA_QUALITY_CHECKS
${tools_dir}/grib_set -s scaleValuesBy=100 $input1 $tempOut
${tools_dir}/grib_set -s scaleValuesBy=100 $input2 $tempOut

# Data quality checks enabled. Commands should fail
export ECCODES_GRIB_DATA_QUALITY_CHECKS=1
set +e
${tools_dir}/grib_set -s scaleValuesBy=100 $input1 $tempOut 2>$tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q 'GRIB1 simple packing: unable to set values' $tempErr
grep -q 'outside allowable limits' $tempErr

set +e
${tools_dir}/grib_set -s scaleValuesBy=100 $input2 $tempOut 2>$tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q 'GRIB2 simple packing: unable to set values' $tempErr
grep -q 'outside allowable limits' $tempErr


# Clean up
rm -f $tempOut $tempErr

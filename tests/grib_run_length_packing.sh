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

label="grib_run_length_packing_test"
tempGrib=temp.$label.grib
tempLog=temp.$label.log

input=$data_dir/run_length_packing.grib2
if [ ! -f "$input" ]; then
    echo " Input data file missing: $input"
    exit 0
fi

${tools_dir}/grib_dump $input

stats=$(${tools_dir}/grib_get -F%.5f -p packingType,min,max,avg $input)
[ "$stats" = "grid_run_length 1.00000 3.00000 1.00303" ]

grib_check_key_equals $input numberOfMissing 71493
grib_check_key_equals $input missingValuesPresent 1

rm -f $tempGrib $tempLog

#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

label="grib_ecc-604"
temp_dir=tempdir.${label}

validate()
{
    echo "Checking every output file is identical..."
    set +x
    res=`cksum $OUTPUT/output_file_* | awk '{print $1}' | sort -u`
    set -x
    [ "$res" = "2572910830" ]
}

NUM_THREADS=8
NUM_ITER=300
OUTPUT=output
input=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl

rm -fr $temp_dir
mkdir -p $temp_dir
cd $temp_dir

# Test 01
mkdir -p $OUTPUT
time ${test_dir}/grib_ecc-604-1 par $input $NUM_THREADS $NUM_ITER
validate

# Test 02
rm -fr $OUTPUT
mkdir -p $OUTPUT
time ${test_dir}/grib_ecc-604-2 par $input $NUM_THREADS $NUM_ITER
validate


# Clean up
cd $test_dir
rm -fr $temp_dir

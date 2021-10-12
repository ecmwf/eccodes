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

label="bufr_threads_ecc-604"
temp_dir=tempdir.${label}

NUM_THREADS=3
NUM_ITER=5
OUTPUT=output
PROG="${test_dir}/bufr_threads_ecc-604"

validate()
{
    echo "Checking every output file is identical..."
    # Get checksum of first file
    ck1=`cksum $OUTPUT/output_file_0-0.bufr | awk '{print $1}'`
    set +x
    # Get checksum of all of them and sort unique
    res=`cksum $OUTPUT/output_file_* | awk '{print $1}' | sort -u`
    set -x
    # Should be the same as the first
    [ "$res" = "$ck1" ]
}
process()
{
    input=$1 # The input BUFR file

    # Test 01: Clone + output
    # ------------------------
    rm -fr $OUTPUT;   mkdir -p $OUTPUT
    time $PROG -c -w par $input $NUM_THREADS $NUM_ITER
    validate

    # Test 02: No clone + output
    # --------------------------
    rm -fr $OUTPUT;   mkdir -p $OUTPUT
    time $PROG -w par $input $NUM_THREADS $NUM_ITER
    validate

    # Test 03: Clone + no output
    # ---------------------------
    rm -fr $OUTPUT
    time $PROG -c par $input $NUM_THREADS $NUM_ITER
    # Nothing to validate as there is no output
}
###################################################
rm -fr $temp_dir
mkdir -p $temp_dir
cd $temp_dir

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
for bf in ${bufr_files}; do
    b=${data_dir}/bufr/$bf
    echo "Doing $b"
    process $b
done

# Clean up
cd $test_dir
rm -fr $temp_dir

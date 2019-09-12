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

NUM_THREADS=3
NUM_ITER=10
OUTPUT=output

validate()
{
    echo "Checking every output file is identical..."
    # Get checksum of first file
    ck1=`cksum $OUTPUT/output_file_0-0.grib | awk '{print $1}'`
    set +x
    # Get checksum of all of them and sort unique
    res=`cksum $OUTPUT/output_file_* | awk '{print $1}' | sort -u`
    set -x
    # Should be the same as the first
    [ "$res" = "$ck1" ]
}
process()
{
    input=$1 # The input GRIB file

    # Test 01: Clone + output
    # ------------------------
    rm -fr $OUTPUT;   mkdir -p $OUTPUT
    time ${test_dir}/grib_ecc-604 -c -w par $input $NUM_THREADS $NUM_ITER
    validate

    # Test 02: No clone + output
    # --------------------------
    rm -fr $OUTPUT;   mkdir -p $OUTPUT
    time ${test_dir}/grib_ecc-604 -w par $input $NUM_THREADS $NUM_ITER
    validate

    # Test 03: Clone + dump + no output
    # ---------------------------------
    rm -fr $OUTPUT
    time ${test_dir}/grib_ecc-604 -c -d par $input $NUM_THREADS $NUM_ITER
    # Nothing to validate as there is no output
}
###################################################
rm -fr $temp_dir
mkdir -p $temp_dir
cd $temp_dir

GRIB1_INPUTS="
  $ECCODES_SAMPLES_PATH/gg_sfc_grib1.tmpl
  ${data_dir}/gen_bitmap.grib
  ${data_dir}/spectral_complex.grib1
  ${data_dir}/gen_ext.grib
  ${data_dir}/gen.grib
  ${data_dir}/gen_ext_spd_2.grib"

GRIB2_INPUTS="
  $ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
  ${data_dir}/reduced_gaussian_sub_area.grib2
  ${data_dir}/test_file.grib2
  ${data_dir}/sample.grib2"

if [ $HAVE_JPEG -eq 1 ]; then
    echo "Adding extra files (HAVE_JPEG=1)"
    GRIB2_INPUTS="${data_dir}/jpeg.grib2 ${data_dir}/reduced_gaussian_surface_jpeg.grib2 "$GRIB2_INPUTS
fi
if [ $HAVE_AEC -eq 1 ]; then
    GRIB2_INPUTS=$GRIB2_INPUTS" ${data_dir}/ccsds.grib2 "
fi

for gf in $GRIB1_INPUTS $GRIB2_INPUTS; do
    process $gf
done

# Clean up
cd $test_dir
rm -fr $temp_dir

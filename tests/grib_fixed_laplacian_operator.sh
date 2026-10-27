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

# ---------------------------------------------------------
# Spectral complex packing: fixed laplacian operator for
# geopotential and lnsp on model levels (ECMWF only)
# ---------------------------------------------------------

label="grib_fixed_laplacian_operator_test"
tempGrib=temp.$label.grib
tempOut=temp.$label.out.grib

sample_grib2=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl

# Fitted laplacian operator of the sample values
${tools_dir}/grib_set -r -s bitsPerValue=16 $sample_grib2 $tempOut
fitted=`${tools_dir}/grib_get -p laplacianScalingFactor $tempOut`
[ "$fitted" != "500000" ]

# z and lnsp on model levels: fixed
for paramId in 129 152; do
    ${tools_dir}/grib_set -s paramId=$paramId $sample_grib2 $tempGrib
    ${tools_dir}/grib_set -r -s bitsPerValue=16 $tempGrib $tempOut
    grib_check_key_equals $tempOut laplacianScalingFactor 500000
done

# Other parameters on model levels: fitted
${tools_dir}/grib_set -s paramId=138 $sample_grib2 $tempGrib
${tools_dir}/grib_set -r -s bitsPerValue=16 $tempGrib $tempOut
grib_check_key_equals $tempOut laplacianScalingFactor $fitted

# z on pressure levels: fitted
${tools_dir}/grib_set -s paramId=129,typeOfFirstFixedSurface=100,level=500 $sample_grib2 $tempGrib
${tools_dir}/grib_set -r -s bitsPerValue=16 $tempGrib $tempOut
grib_check_key_equals $tempOut laplacianScalingFactor $fitted

# Other centres: fitted
${tools_dir}/grib_set -s paramId=129,centre=78 $sample_grib2 $tempGrib
${tools_dir}/grib_set -r -s bitsPerValue=16 $tempGrib $tempOut
grib_check_key_equals $tempOut laplacianScalingFactor $fitted

# An explicitly set laplacian operator takes precedence
${tools_dir}/grib_set -s paramId=129 $sample_grib2 $tempGrib
${tools_dir}/grib_set -r -s computeLaplacianOperator=0,laplacianOperator=0.7,bitsPerValue=16 $tempGrib $tempOut
grib_check_key_equals $tempOut laplacianScalingFactor 700000

# Clean up
rm -f $tempGrib $tempOut

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
# This is the test for JIRA issue ECC-2366
# Spectral complex packing: fixed laplacian operator for
# geopotential and lnsp on model levels (ECMWF only)
# ---------------------------------------------------------

label="grib_ecc-2366_test"
tempGrib=temp.$label.grib
tempOut=temp.$label.out.grib
tempConv=temp.$label.conv.grib

# ---------------------------------------------------------
# Spherical harmonics (GRIB1 and GRIB2)
# ---------------------------------------------------------
check_spherical_harmonics()
{
    sample=$1   # Model-level spherical harmonics sample
    key=$2      # P (GRIB1) or laplacianScalingFactor (GRIB2)
    fixed=$3    # Coded value of a laplacianOperator of 0.5
    manual=$4   # Coded value of a laplacianOperator of 0.7
    pl=$5       # typeOfLevel key/value for pressure levels

    # Fitted laplacian operator of the sample values
    ${tools_dir}/grib_set -r -s bitsPerValue=16 $sample $tempOut
    fitted=`${tools_dir}/grib_get -p $key $tempOut`
    [ "$fitted" != "$fixed" ]

    # z and lnsp on model levels: fixed
    for paramId in 129 152; do
        ${tools_dir}/grib_set -s paramId=$paramId $sample $tempGrib
        ${tools_dir}/grib_set -r -s bitsPerValue=16 $tempGrib $tempOut
        grib_check_key_equals $tempOut $key,laplacianOperator "$fixed 0.5"
        # Data must still decode correctly
        ${tools_dir}/grib_compare -c values -A 1e-4 $tempGrib $tempOut
    done

    # Other parameters on model levels: fitted
    ${tools_dir}/grib_set -s paramId=138 $sample $tempGrib
    ${tools_dir}/grib_set -r -s bitsPerValue=16 $tempGrib $tempOut
    grib_check_key_equals $tempOut $key $fitted

    # z on pressure levels: fitted
    ${tools_dir}/grib_set -s paramId=129,$pl,level=500 $sample $tempGrib
    ${tools_dir}/grib_set -r -s bitsPerValue=16 $tempGrib $tempOut
    grib_check_key_equals $tempOut $key $fitted

    # Other centres: fitted
    ${tools_dir}/grib_set -s paramId=129,centre=78 $sample $tempGrib
    ${tools_dir}/grib_set -r -s bitsPerValue=16 $tempGrib $tempOut
    grib_check_key_equals $tempOut $key $fitted

    # An explicitly set laplacian operator takes precedence
    ${tools_dir}/grib_set -s paramId=129 $sample $tempGrib
    ${tools_dir}/grib_set -r -s computeLaplacianOperator=0,laplacianOperator=0.7,bitsPerValue=16 $tempGrib $tempOut
    grib_check_key_equals $tempOut $key,laplacianOperator "$manual 0.7"
    ${tools_dir}/grib_compare -c values -A 1e-4 $tempGrib $tempOut
}

check_spherical_harmonics $ECCODES_SAMPLES_PATH/sh_ml_grib1.tmpl P 500 700 indicatorOfTypeOfLevel=100
check_spherical_harmonics $ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl laplacianScalingFactor 500000 700000 typeOfFirstFixedSurface=100

# ---------------------------------------------------------
# Conversion between editions repacks the data
# ---------------------------------------------------------
for paramId in 129 152; do
    # GRIB1 -> GRIB2
    ${tools_dir}/grib_set -s paramId=$paramId $ECCODES_SAMPLES_PATH/sh_ml_grib1.tmpl $tempGrib
    ${tools_dir}/grib_set -s edition=2 $tempGrib $tempConv
    grib_check_key_equals $tempConv edition,paramId,typeOfLevel,laplacianScalingFactor,laplacianOperator "2 $paramId hybrid 500000 0.5"
    ${tools_dir}/grib_compare -c values -A 1e-4 $tempGrib $tempConv

    # GRIB2 -> GRIB1
    ${tools_dir}/grib_set -s paramId=$paramId $ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl $tempGrib
    ${tools_dir}/grib_set -s edition=1 $tempGrib $tempConv
    grib_check_key_equals $tempConv edition,paramId,typeOfLevel,P,laplacianOperator "1 $paramId hybrid 500 0.5"
    ${tools_dir}/grib_compare -c values -A 1e-4 $tempGrib $tempConv
done

# Conversion of another parameter: fitted
${tools_dir}/grib_set -s paramId=138 $ECCODES_SAMPLES_PATH/sh_ml_grib1.tmpl $tempGrib
${tools_dir}/grib_set -s edition=2 $tempGrib $tempConv
result=`${tools_dir}/grib_get -p laplacianScalingFactor $tempConv`
[ "$result" != "500000" ]

# ---------------------------------------------------------
# Bi-Fourier coefficients (limited area models, GRIB2)
# ---------------------------------------------------------
# The sample uses 64-bit IEEE for the unpacked subset which needs
# sizeof(long) == sizeof(double). So on Windows, this part is disabled
# Real fix: in DataG2BifourierPacking, handle bytes == 8 with a 64-bit type instead of unsigned long. Read and write the 8 bytes into a uint64_t and memcpy to and from
# double. hpos is always byte-aligned there, so this is simple. Alternatively, change grib_ieee64_to_long/grib_long_to_ieee64 to use uint64_t. That would fix the root cause
# and could re-enable the grib_sh_ieee64 test and the sample check on Windows.
if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: Bi-Fourier tests are currently disabled on Windows"
    rm -f $tempGrib $tempOut $tempConv
    exit 0
fi

sample=$ECCODES_SAMPLES_PATH/lambert_bf_grib2.tmpl
# A sub-truncation smaller than the truncation so that the laplacian operator is applied
subset="biFourierResolutionSubSetParameterN=2,biFourierResolutionSubSetParameterM=2,bitsPerValue=16"

# Fitted laplacian operator of the sample values (centre is not ECMWF)
${tools_dir}/grib_set -r -s $subset $sample $tempOut
fitted=`${tools_dir}/grib_get -p laplacianScalingFactor $tempOut`
[ "$fitted" != "500000" ]

# z and lnsp on model levels: fixed
for paramId in 129 152; do
    ${tools_dir}/grib_set -s centre=98,paramId=$paramId,typeOfLevel=hybrid,level=1 $sample $tempGrib
    ${tools_dir}/grib_set -r -s $subset $tempGrib $tempOut
    grib_check_key_equals $tempOut laplacianScalingFactor,laplacianOperator "500000 0.5"
    ${tools_dir}/grib_compare -c values -A 1e-3 $tempGrib $tempOut
done

# Other parameters on model levels: fitted
${tools_dir}/grib_set -s centre=98,paramId=138,typeOfLevel=hybrid,level=1 $sample $tempGrib
${tools_dir}/grib_set -r -s $subset $tempGrib $tempOut
grib_check_key_equals $tempOut laplacianScalingFactor $fitted

# An explicitly set laplacian operator takes precedence
${tools_dir}/grib_set -s centre=98,paramId=129,typeOfLevel=hybrid,level=1 $sample $tempGrib
${tools_dir}/grib_set -r -s computeLaplacianOperator=0,laplacianOperator=0.7,$subset $tempGrib $tempOut
grib_check_key_equals $tempOut laplacianScalingFactor,laplacianOperator "700000 0.7"

# Clean up
rm -f $tempGrib $tempOut $tempConv

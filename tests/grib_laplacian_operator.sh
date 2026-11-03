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

# Test the Laplacian operator used in spectral complex packing and the associated keys:
#   laplacianOperator, laplacianOperatorIsSet, computeLaplacianOperator, truncateLaplacian,
#   P/PUnset (GRIB1), laplacianScalingFactor/laplacianScalingFactorUnset (GRIB2)
#
# Modes:
#   computeLaplacianOperator=1 (default):  laplacianOperator is computed from the data on encoding
#   computeLaplacianOperator=0:            the coded laplacianOperator is used, unless it is unset
#   GRIBEX mode (ECCODES_GRIBEX_MODE_ON):  computeLaplacianOperator defaults to 0

label="grib_laplacian_operator_test"
tempGrib=temp.${label}.grib
tempGrib2=temp.${label}.2.grib
tempFilt=temp.${label}.filt
tempText=temp.${label}.txt

grib1=${data_dir}/spherical_model_level.grib1
grib2=${data_dir}/spherical_model_level.grib2

# ------------------------------------------------------------------
# Samples: initial state of the keys
# ------------------------------------------------------------------
for sample in sh_ml_grib1 sh_ml_grib2; do
    input=$ECCODES_SAMPLES_PATH/$sample.tmpl
    grib_check_key_equals $input packingType,laplacianOperator "spectral_complex 1.29"
    grib_check_key_equals $input computeLaplacianOperator,truncateLaplacian,laplacianOperatorIsSet "1 0 0"

    # In GRIBEX mode the coded value is used, hence it is regarded as set
    result=$(ECCODES_GRIBEX_MODE_ON=1 ${tools_dir}/grib_get -p computeLaplacianOperator,laplacianOperatorIsSet $input)
    [ "$result" = "0 1" ]
done
grib_check_key_equals $ECCODES_SAMPLES_PATH/sh_ml_grib1.tmpl P,PUnset "1290 -32767"
grib_check_key_equals $ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl laplacianScalingFactor,laplacianScalingFactorUnset "1290000 -2147483647"

# ------------------------------------------------------------------
# GRIB1: scaling (P is in units of 10^-3) and laplacianOperatorIsSet
# ------------------------------------------------------------------
cat > $tempFilt <<EOF
    assert( laplacianOperator == 1.385 );
    assert( P == 1385 );

    # Default: computed on encoding, so not regarded as set
    assert( computeLaplacianOperator == 1 );
    assert( laplacianOperatorIsSet == 0 );

    set computeLaplacianOperator = 0;
    assert( laplacianOperatorIsSet == 1 );
    set P = PUnset;
    assert( laplacianOperatorIsSet == 0 );
    set P = 1385;
    assert( laplacianOperatorIsSet == 1 );

    # Encoding laplacianOperator sets P
    set laplacianOperator = 0.5;
    assert( P == 500 );
    set P = 250;
    assert( laplacianOperator == 0.25 );

    # truncateLaplacian: truncate rather than round
    assert( truncateLaplacian == 0 );
    set laplacianOperator = 0.1239;
    assert( P == 124 );
    set laplacianOperator = -0.1239;
    assert( P == -124 );
    set truncateLaplacian = 1;
    set laplacianOperator = 0.1239;
    assert( P == 123 );
    set laplacianOperator = -0.1239;
    assert( P == -123 );
EOF
${tools_dir}/grib_filter $tempFilt $grib1

# ------------------------------------------------------------------
# GRIB2: scaling (laplacianScalingFactor is in units of 10^-6) and laplacianOperatorIsSet
# ------------------------------------------------------------------
cat > $tempFilt <<EOF
    assert( laplacianScalingFactor == 677944 );

    assert( computeLaplacianOperator == 1 );
    assert( laplacianOperatorIsSet == 0 );

    set computeLaplacianOperator = 0;
    assert( laplacianOperatorIsSet == 1 );
    set laplacianScalingFactor = laplacianScalingFactorUnset;
    assert( laplacianOperatorIsSet == 0 );
    set laplacianScalingFactor = 677944;
    assert( laplacianOperatorIsSet == 1 );

    set laplacianOperator = 0.5;
    assert( laplacianScalingFactor == 500000 );
    set laplacianScalingFactor = 123;
    assert( laplacianOperator == 0.000123 );

    assert( truncateLaplacian == 0 );
    set laplacianOperator = 0.1234569;
    assert( laplacianScalingFactor == 123457 );
    set laplacianOperator = -0.1234569;
    assert( laplacianScalingFactor == -123457 );
    set truncateLaplacian = 1;
    set laplacianOperator = 0.1234569;
    assert( laplacianScalingFactor == 123456 );
    set laplacianOperator = -0.1234569;
    assert( laplacianScalingFactor == -123456 );
EOF
${tools_dir}/grib_filter $tempFilt $grib2

# Constants cannot be changed
set +e
${tools_dir}/grib_set -s PUnset=0 $grib1 $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
set +e
${tools_dir}/grib_set -s laplacianScalingFactorUnset=0 $grib2 $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]

# ------------------------------------------------------------------
# Encoding modes (GRIB1 and GRIB2)
# The -r option forces the data to be repacked
# ------------------------------------------------------------------
check_modes()
{
    input=$1
    key=$2          # P or laplacianScalingFactor
    unset_value=$3  # Value of PUnset or laplacianScalingFactorUnset
    orig_value=$4   # Coded value in input
    computed=$5     # Value computed by ecCodes from the data
    manual=$6       # Coded value of a manual laplacianOperator=0.5

    # Default mode: computed from the data
    ${tools_dir}/grib_set -r -s bitsPerValue=16 $input $tempGrib
    grib_check_key_equals $tempGrib $key $computed

    # Default mode: a manually set laplacianOperator is overridden
    ${tools_dir}/grib_set -r -s laplacianOperator=0.5 $input $tempGrib
    grib_check_key_equals $tempGrib $key $computed

    # Manual mode: the given laplacianOperator is used for packing
    ${tools_dir}/grib_set -r -s computeLaplacianOperator=0,laplacianOperator=0.5 $input $tempGrib
    grib_check_key_equals $tempGrib $key,laplacianOperator "$manual 0.5"
    # Data must still decode correctly
    ${tools_dir}/grib_compare -c values -A 1e-4 $input $tempGrib

    # Manual mode: the coded laplacianOperator of the input is kept
    ${tools_dir}/grib_set -r -s computeLaplacianOperator=0 $input $tempGrib
    grib_check_key_equals $tempGrib $key $orig_value

    # Manual mode with unset value: falls back to computing it
    ${tools_dir}/grib_set -r -s computeLaplacianOperator=0,$key=$unset_value $input $tempGrib
    grib_check_key_equals $tempGrib $key $computed

    # GRIBEX mode: manual by default
    ECCODES_GRIBEX_MODE_ON=1 ${tools_dir}/grib_set -r -s laplacianOperator=0.5 $input $tempGrib
    grib_check_key_equals $tempGrib $key $manual
    ECCODES_GRIBEX_MODE_ON=1 ${tools_dir}/grib_set -r -s bitsPerValue=16 $input $tempGrib
    grib_check_key_equals $tempGrib $key $orig_value

    # GRIBEX mode: computing can be forced
    ECCODES_GRIBEX_MODE_ON=1 ${tools_dir}/grib_set -r -s computeLaplacianOperator=1 $input $tempGrib
    grib_check_key_equals $tempGrib $key $computed

    # Without repacking only the header is changed
    ${tools_dir}/grib_set -s laplacianOperator=0.5 $input $tempGrib
    grib_check_key_equals $tempGrib $key $manual

    # ECC-2328: Constant fields reset laplacianOperator to 0
    ${tools_dir}/grib_set -d 3 $input $tempGrib
    grib_check_key_equals $tempGrib $key,laplacianOperator "0 0"

    # truncateLaplacian also applies when the operator is computed
    ${tools_dir}/grib_set -r -s truncateLaplacian=1 $input $tempGrib
    ${tools_dir}/grib_set -r -s truncateLaplacian=0 $input $tempGrib2
    trunc=$(${tools_dir}/grib_get -p $key $tempGrib)
    round=$(${tools_dir}/grib_get -p $key $tempGrib2)
    [ $trunc -le $round ]
}

check_modes $grib1 P -32767 1385 678 500
check_modes $grib2 laplacianScalingFactor -2147483647 677944 677957 500000

# ------------------------------------------------------------------
# Changing computeLaplacianOperator immediately before setting the values
# The last setting before encoding the values decides the mode
# ------------------------------------------------------------------
sh_values=$(awk 'BEGIN{for(i=1;i<=42;i++){printf "%s%.4f", (i>1?",":""), 100.0/(i*i)*((i%2)?1:-1)}}')

check_compute_before_values()
{
    input=$1
    key=$2       # P or laplacianScalingFactor
    computed=$3  # Coded value of the operator computed from sh_values
    scale=$4     # Coded value of laplacianOperator=1

    cat > $tempFilt <<EOF
    set J = 5; set K = 5; set M = 5;
    set JS = 2; set KS = 2; set MS = 2;

    set computeLaplacianOperator = 1;
    set values = { $sh_values };
    assert( numberOfValues == 42 );
    assert( $key == $computed );

    set laplacianOperator = 0.5;
    set computeLaplacianOperator = 0;
    assert( laplacianOperatorIsSet == 1 );
    set values = { $sh_values };
    assert( $key == 0.5 * $scale );

    set computeLaplacianOperator = 1;
    assert( laplacianOperatorIsSet == 0 );
    set values = { $sh_values };
    assert( $key == $computed );

    # Switching off and on again: computed
    set computeLaplacianOperator = 0;
    set laplacianOperator = 0.25;
    set computeLaplacianOperator = 1;
    set values = { $sh_values };
    assert( $key == $computed );

    # Switching on and off again: manual
    set laplacianOperator = 0.25;
    set computeLaplacianOperator = 1;
    set computeLaplacianOperator = 0;
    set values = { $sh_values };
    assert( $key == 0.25 * $scale );

    # Setting the values does not reset computeLaplacianOperator
    assert( computeLaplacianOperator == 0 );
    set values = { $sh_values };
    assert( $key == 0.25 * $scale );
    write;
EOF
    ${tools_dir}/grib_filter -o $tempGrib $tempFilt $input
    # computeLaplacianOperator is transient: back to the default on decoding
    grib_check_key_equals $tempGrib computeLaplacianOperator,laplacianOperator,laplacianOperatorIsSet "1 0.25 0"

    # An explicit computeLaplacianOperator overrides GRIBEX mode
    ECCODES_GRIBEX_MODE_ON=1 ${tools_dir}/grib_filter -o $tempGrib $tempFilt $input
    result=$(ECCODES_GRIBEX_MODE_ON=1 ${tools_dir}/grib_get -p computeLaplacianOperator,laplacianOperator,laplacianOperatorIsSet $tempGrib)
    [ "$result" = "0 0.25 1" ]
}

check_compute_before_values $ECCODES_SAMPLES_PATH/sh_ml_grib1.tmpl P 986 1000
check_compute_before_values $ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl laplacianScalingFactor 986010 1000000

# ------------------------------------------------------------------
# Spectral simple packing has no Laplacian operator
# ------------------------------------------------------------------
${tools_dir}/grib_set -r -s packingType=spectral_simple $grib1 $tempGrib
grib_check_key_equals $tempGrib packingType spectral_simple
set +e
${tools_dir}/grib_get -p laplacianOperator $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Key/value not found" $tempText

# ------------------------------------------------------------------
# GRIB2 LAM Bi-Fourier complex packing (template 5.53)
# ------------------------------------------------------------------
sample_bf=$ECCODES_SAMPLES_PATH/lambert_bf_grib2.tmpl
grib_check_key_equals $sample_bf packingType,dataRepresentationTemplateNumber "bifourier_complex 53"
# computeLaplacianOperator is always 1 for this template, even in GRIBEX mode
grib_check_key_equals $sample_bf computeLaplacianOperator,truncateLaplacian,laplacianOperatorIsSet "1 0 0"
result=$(ECCODES_GRIBEX_MODE_ON=1 ${tools_dir}/grib_get -p computeLaplacianOperator $sample_bf)
[ "$result" = "1" ]

bf_values=$(awk 'BEGIN{for(i=1;i<=112;i++){printf "%s%.4f", (i>1?",":""), 100.0/(1+i)*((i%2)?1:-1)}}')
bf_const=$(awk 'BEGIN{for(i=1;i<=112;i++){printf "%s3", (i>1?",":"")}}')
cat > $tempFilt <<EOF
    # Pack only the first coefficients unpacked so the operator can be computed
    set biFourierResolutionSubSetParameterN = 1;
    set biFourierResolutionSubSetParameterM = 1;
    assert( numberOfValues == 112 );

    # Default mode: computed from the data
    set values = { $bf_values };
    assert( laplacianOperatorIsSet == 0 );
    assert( laplacianScalingFactor == 457777 );

    # Default mode: a manually set laplacianOperator is overridden
    set laplacianOperator = 0.5;
    set values = { $bf_values };
    assert( laplacianScalingFactor == 457777 );

    # Manual mode
    set computeLaplacianOperator = 0;
    set laplacianOperator = 0.5;
    assert( laplacianOperatorIsSet == 1 );
    set values = { $bf_values };
    assert( laplacianScalingFactor == 500000 );

    # Manual mode with unset value: falls back to computing it
    set laplacianScalingFactor = laplacianScalingFactorUnset;
    assert( laplacianOperatorIsSet == 0 );
    set values = { $bf_values };
    assert( laplacianScalingFactor == 457777 );

    # truncateLaplacian
    set truncateLaplacian = 1;
    set laplacianOperator = 0.1234569;
    assert( laplacianScalingFactor == 123456 );
    set truncateLaplacian = 0;
    set laplacianOperator = 0.1234569;
    assert( laplacianScalingFactor == 123457 );

    # Constant field
    set computeLaplacianOperator = 1;
    set values = { $bf_const };
    assert( laplacianOperator == 0 );
EOF
${tools_dir}/grib_filter $tempFilt $sample_bf

# ECC-1207: All coefficients unpacked so the operator is 0
cat > $tempFilt <<EOF
    set values = { $bf_values };
    assert( laplacianOperator == 0 );
EOF
${tools_dir}/grib_filter $tempFilt $sample_bf


# Clean up
rm -f $tempGrib $tempGrib2 $tempFilt $tempText

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

label="grib_spectral_test"

tempFilt=temp.${label}.filt
tempGrib=temp.${label}.grib

input_complex=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl

$EXEC ${test_dir}/grib_spectral $input_complex $tempGrib

rm -f $tempGrib

# Now try spectral simple
tempSimple=temp.$label.simple.grib
${tools_dir}/grib_set  -rs packingType=spectral_simple $input_complex $tempSimple
$EXEC ${test_dir}/grib_spectral $tempSimple $tempGrib

# GRIB1: octet_number pack_long
input_complex=$ECCODES_SAMPLES_PATH/sh_ml_grib1.tmpl
${tools_dir}/grib_set  -s Nassigned=1 $input_complex $tempGrib

# Encode a spectral sample starting from GRIB2.tmpl
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
cat > $tempFilt <<EOF
    set numberOfDataPoints = 6;
    set numberOfValues = 6;
    set bitsPerValue = 16;
    set values = {33, 0, 1, 0, 2, 0};
    set typeOfFirstFixedSurface = 105;
    set scaleFactorOfFirstFixedSurface = 0;
    set scaledValueOfFirstFixedSurface = 0;

    set gridDefinitionTemplateNumber=50;
    set J=1;
    set K=1;
    set M=1;
    set spectralType = 1;
    set spectralMode = 1;

    set dataRepresentationTemplateNumber=51;
    write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample2
${tools_dir}/grib_get_data $tempGrib
${tools_dir}/grib_dump $tempGrib
grib_check_key_equals $tempGrib packingType spectral_complex
grib_check_key_equals $tempGrib gridType sh

rm -f $tempSimple $tempFilt $tempGrib

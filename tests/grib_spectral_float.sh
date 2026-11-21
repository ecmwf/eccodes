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

# Single-precision get/set of values for spectral fields

label="grib_spectral_float_test"
temp=temp.$label.grib

files=""
for s in sh_ml_grib1 sh_ml_grib2 sh_pl_grib1 sh_pl_grib2 sh_sfc_grib1 sh_sfc_grib2; do
    files="$files $ECCODES_SAMPLES_PATH/$s.tmpl"
done
files="$files ${data_dir}/spherical_pressure_level.grib1 ${data_dir}/spherical_pressure_level.grib2"
files="$files ${data_dir}/spherical_model_level.grib1 ${data_dir}/spherical_model_level.grib2"

for f in $files; do
    # spectral_complex
    $EXEC ${test_dir}/grib_spectral_float $f

    # spectral_simple
    ${tools_dir}/grib_set -r -s packingType=spectral_simple $f $temp
    grib_check_key_equals $temp packingType spectral_simple
    $EXEC ${test_dir}/grib_spectral_float $temp

    # spectral_complex with a different number of bits per value
    ${tools_dir}/grib_set -s bitsPerValue=24 $f $temp
    $EXEC ${test_dir}/grib_spectral_float $temp
done

rm -f $temp

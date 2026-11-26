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

# The simple_packing library must encode GRIB1 and GRIB2 simple packing exactly like the legacy
# code (the default; ECCODES_USE_SP=1 selects the library)

label="grib_simple_packing_legacy_test"
tempLib=temp.$label.lib.grib
tempLegacy=temp.$label.legacy.grib

files="simple.grib regular_latlon_surface.grib1 regular_latlon_surface.grib2
       reduced_gaussian_model_level.grib1 reduced_gaussian_model_level.grib2
       regular_gaussian_pressure_level.grib1 regular_gaussian_pressure_level.grib2"

# Settings that select the different optimisers (bitsPerValue > 56 is not compared: the legacy
# decoder is wrong for some of these values)
settings="changeDecimalPrecision=0 changeDecimalPrecision=1 changeDecimalPrecision=2 changeDecimalPrecision=4
          bitsPerValue=1 bitsPerValue=8 bitsPerValue=12 bitsPerValue=16 bitsPerValue=24 bitsPerValue=32 bitsPerValue=48
          decimalScaleFactor=2,bitsPerValue=0"

for f in $files; do
    for s in $settings; do
        ECCODES_USE_SP=1 ${tools_dir}/grib_set -r -s $s ${data_dir}/$f $tempLib
        ${tools_dir}/grib_set -r -s $s ${data_dir}/$f $tempLegacy
        if ! cmp -s $tempLib $tempLegacy; then
            echo "Different encoding of $f with $s:"
            ${tools_dir}/grib_compare -b totalLength $tempLib $tempLegacy || true
            exit 1
        fi
    done
done

rm -f $tempLib $tempLegacy

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

# Define a common label for all the tmp files
label="grib_dump_samples_test"
temp=${label}".temp"

# Test selected sample GRIB files
samples="
    GRIB1.tmpl
    GRIB2.tmpl
    sh_ml_grib1.tmpl
    sh_ml_grib2.tmpl
    reduced_gg_pl_48_grib1.tmpl
    reduced_gg_pl_48_grib2.tmpl
    regular_ll_sfc_grib1.tmpl
    regular_ll_sfc_grib2.tmpl
"
for file in $samples; do
  sf="$ECCODES_SAMPLES_PATH/$file"
  ${tools_dir}/grib_dump -O $sf >/dev/null
done

# Test grib_dump with -t option
${tools_dir}/grib_dump -O -t $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $temp
grep -q "signed (int) scaleFactorOfFirstFixedSurface" $temp
grep -q "codetable (int) typeOfSecondFixedSurface" $temp
grep -q "ieeefloat (double) referenceValue" $temp
grep -q "unsigned (int) numberOfSection" $temp


rm -f $temp

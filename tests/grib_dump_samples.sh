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

# Define a common label for all the tmp files
label="grib_dump_samples_test"
temp=${label}".temp"

sample_ccsds="$ECCODES_SAMPLES_PATH/ccsds_grib2.tmpl"

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

for sfile in $samples; do
  sample="$ECCODES_SAMPLES_PATH/$sfile"
  ${tools_dir}/grib_dump -Oa $sample >/dev/null
  ${tools_dir}/grib_dump -atH $sample >/dev/null
done

# Test grib_dump with -t option
${tools_dir}/grib_dump -O -t $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $temp
grep -q "signed (int) scaleFactorOfFirstFixedSurface" $temp
grep -q "codetable (int) typeOfSecondFixedSurface" $temp
grep -q "ieeefloat (double) referenceValue" $temp
grep -q "unsigned (int) numberOfSection" $temp

# Extra tests for CCSDS
if [ $HAVE_AEC -eq 1 ]; then
    ${tools_dir}/grib_dump -O $sample_ccsds >/dev/null
    rm -f $temp
    echo 'set values = { 55.0161, 66.666, 99.7008 };write;' |\
         ${tools_dir}/grib_filter -o $temp - $sample_ccsds
    grib_check_key_equals $temp packingType,numberOfValues 'grid_ccsds 3'
    stats=`${tools_dir}/grib_get -M -F%.4f -p min,max $temp`
    [ "$stats" = "55.0161 99.7008" ]
    ${tools_dir}/grib_dump -O $temp
fi

# Check ifs_samples/grib1_mlgrib2_ccsds
# Those that are GRIB2 and for grid-point data must be CCSDS packed
g2_samples="gg_ml.tmpl gg_sfc_grib2.tmpl"
for s in $g2_samples; do
    sf=${proj_dir}/ifs_samples/grib1_mlgrib2_ccsds/$s
    grib_check_key_equals $sf "edition,packingType" "2 grid_ccsds"
done


rm -f $temp

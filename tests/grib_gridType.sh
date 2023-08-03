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

REDIRECT=/dev/null
label="grib_gridType_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempText=temp.$label.txt
rm -f $tempGrib $tempFilt $tempText

${tools_dir}/grib_set -s gridType=regular_gg ${data_dir}/reduced_latlon_surface.grib1 ${tempGrib} > $REDIRECT

gridType=`${tools_dir}/grib_get -p gridType $tempGrib`
if [ $gridType != "regular_gg" ]; then
    echo "Unable to change from reduced_latlon to regular_gg"
    echo $gridType
    exit 1
fi

rm -f $tempGrib

${tools_dir}/grib_set -s gridType=reduced_gg ${data_dir}/regular_gaussian_pressure_level.grib1 ${tempGrib} > $REDIRECT

gridType=`${tools_dir}/grib_get -p gridType $tempGrib`
if [ $gridType != "reduced_gg" ]; then
    echo "Unable to change from regular_gg to reduced_gg"
    echo $gridType
    exit 1
fi

rm -f $tempGrib

# Key gridName
# ---------------
for f in $ECCODES_SAMPLES_PATH/regular_gg_ml_grib*tmpl; do
    gname=`${tools_dir}/grib_get -p gridName $f`
    [ "$gname" = "F32" ]
done
for f in $ECCODES_SAMPLES_PATH/reduced_gg_pl_512_grib*.tmpl; do
    gname=`${tools_dir}/grib_get -p gridName $f`
    [ "$gname" = "N512" ]
done

# ECC-1638
# ---------------
cat > $tempFilt <<EOF
 set tablesVersion=31;
 set Ni=2;
 set Nj=3;
 set gridDefinitionTemplateNumber=4;
 set longitude = { 11,  12 };
 set latitude  = {-33, -20, 0};
 set values = {1.7, 6.7, -9.0, 11.1, 3.44, 5.55};
 write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl
set +e
${tools_dir}/grib_ls -l 0,0 $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Nearest neighbour functionality is not supported for grid: Variable resolution latitude/longitude" $tempText

# Check the keys isSpectral and isGridded
# ----------------------------------------
grib2_grid_types="
 regular_ll
 reduced_ll
 rotated_ll
 stretched_ll
 stretched_rotated_ll
 mercator
 transverse_mercator
 polar_stereographic
 lambert
 albers
 regular_gg
 reduced_gg
 rotated_gg
 reduced_rotated_gg
 stretched_gg
 reduced_stretched_gg
 stretched_rotated_gg
 reduced_stretched_rotated_gg
 regular_rotated_gg
 regular_stretched_gg
 regular_stretched_rotated_gg
 sh
 rotated_sh
 stretched_sh
 stretched_rotated_sh
 space_view
 triangular_grid
 unstructured_grid
 equatorial_azimuthal_equidistant
 azimuth_range
 irregular_latlon
 lambert_azimuthal_equal_area
 lambert_lam
 mercator_lam
 polar_stereographic_lam
 lambert_bf
 mercator_bf
 polar_stereographic_bf
"

sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

for gt in $grib2_grid_types; do
    ${tools_dir}/grib_set -s gridType=$gt $sample2 $tempGrib
    ${tools_dir}/grib_get -p isGridded,isSpectral $tempGrib
done


# Clean up
rm -f $tempGrib $tempFilt $tempText

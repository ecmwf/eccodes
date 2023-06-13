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

label="grib_nearest_test"
temp=temp.$label.txt
tempGrib=temp.$label.grib
tempRef=temp.$label.ref
input_grb=${data_dir}/reduced_gaussian_pressure_level.grib1

# Nearest with decoding the data values
# --------------------------------------
$EXEC ${test_dir}/grib_nearest_test $input_grb > $temp
cat > $tempRef <<EOF
ordering by param,step
1 fields in the fieldset
n,step,param
1 0 t  
Idx	lat	lon	dist	val
4839	-40.46	18.00	259.679	285.527
4838	-40.46	15.00	51.5268	284.074
4719	-37.67	18.00	366.445	289.621
4718	-37.67	15.00	258.597	289.027

EOF
diff $tempRef $temp

# Nearest without decoding the data values
# ----------------------------------------
$EXEC ${test_dir}/grib_nearest_test -n $input_grb > $temp
cat > $tempRef <<EOF
ordering by param,step
1 fields in the fieldset
n,step,param
1 0 t  
Idx	lat	lon	dist
4839	-40.46	18.00	259.679
4838	-40.46	15.00	51.5268
4719	-37.67	18.00	366.445
4718	-37.67	15.00	258.597

EOF
diff $tempRef $temp

$EXEC ${test_dir}/grib_nearest_test -n $ECCODES_SAMPLES_PATH/polar_stereographic_pl_grib2.tmpl


# ECC-1295: regular lat/lon on ellipsoid
# ----------------------------------------
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s shapeOfTheEarth=2 $sample2 $tempGrib
grib_check_key_equals $sample2  earthIsOblate 0
grib_check_key_equals $tempGrib earthIsOblate 1
${tools_dir}/grib_ls -l 0,0 $tempGrib

# reduced lat/lon on ellipsoid
# ----------------------------------------
sample2=$ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib2.tmpl
${tools_dir}/grib_set -s shapeOfTheEarth=4 $sample2 $tempGrib
grib_check_key_equals $sample2  earthIsOblate 0
grib_check_key_equals $tempGrib earthIsOblate 1
${tools_dir}/grib_ls -l 0,0 $tempGrib

# regular gaussian on ellipsoid
# ----------------------------------------
sample2=$ECCODES_SAMPLES_PATH/regular_gg_pl_grib2.tmpl
${tools_dir}/grib_set -s shapeOfTheEarth=5 $sample2 $tempGrib
grib_check_key_equals $sample2   earthIsOblate 0
grib_check_key_equals $tempGrib  earthIsOblate 1
${tools_dir}/grib_ls -l 0,0 $tempGrib

# reduced gaussian on ellipsoid
# ----------------------------------------
sample2=$ECCODES_SAMPLES_PATH/reduced_gg_pl_48_grib2.tmpl
${tools_dir}/grib_set -s shapeOfTheEarth=5 $sample2 $tempGrib
grib_check_key_equals $sample2  earthIsOblate 0
grib_check_key_equals $tempGrib earthIsOblate 1
${tools_dir}/grib_ls -l 0,0 $tempGrib



# IEEE regular lat/lon
# ----------------------------------------
input_grb=${data_dir}/grid_ieee.grib
${tools_dir}/grib_get -F%.3g -l 70,345.2,1 $input_grb > $temp
grep -q "3.24e-08" $temp
${tools_dir}/grib_ls -l 70,345.2,1 $input_grb > $temp
grep -q "Grid Point chosen #4 index=0 " $temp


# CCSDS regular lat/lon
# ----------------------------------------
if [ $HAVE_AEC -eq 1 ]; then
    input_grb=${data_dir}/ccsds.grib2
    ${tools_dir}/grib_get -l 79.0,203.0,1 $input_grb > $temp
    grep -q "103484" $temp
    ${tools_dir}/grib_ls -l 79.0,203.0,1 $input_grb > $temp
    grep -q "Grid Point chosen #2 index=4163 " $temp

    # Constant field
    input_grb=${samp_dir}/ccsds_grib2.tmpl
    ${tools_dir}/grib_get -l 0,0,1 $input_grb > $temp
    grep -q "1" $temp
fi

# JPEG regular lat/lon
# ----------------------------------------
if [ $HAVE_JPEG -eq 1 ]; then
    input_grb=${data_dir}/jpeg.grib2
    res=$(${tools_dir}/grib_get -l 0,0,1 $input_grb | tr -d ' ')
    [ "$res" = "101309" ]
fi

# GRIB2 Complex packing regular lat/lon
# ----------------------------------------
input_grb=${data_dir}/sample.grib2
${tools_dir}/grib_set -r -s packingType=grid_complex $input_grb $tempGrib
${tools_dir}/grib_get -F%3.6g -l 0,0,1 $tempGrib > $temp
grep -q "300.119" $temp
${tools_dir}/grib_ls -l 0,0,1 $tempGrib > $temp
grep -q "Grid Point chosen #2 index=480 " $temp


# Subarea: Reduced Gaussian grid
# ------------------------------
input_grb=${data_dir}/reduced_gaussian_sub_area.grib1
${tools_dir}/grib_get -F%3.6g -l 89.656,220,1 $input_grb > $temp
grep -q "274.038" $temp
${tools_dir}/grib_ls -l 89.656,220,1 $input_grb > $temp
grep -q "Grid Point chosen #1 index=0 " $temp

${tools_dir}/grib_ls -l 0.225,399.6,1 $input_grb > $temp
grep -q "Grid Point chosen #1 index=53563 " $temp

${tools_dir}/grib_get -F%3.6g -l 11.91,366.3,1 $input_grb > $temp
grep -q "299.632" $temp
${tools_dir}/grib_ls -l 11.91,366.3,1 $input_grb > $temp
grep -q "Grid Point chosen #1 index=43089 " $temp


# Clean up
rm -f $temp $tempRef $tempGrib

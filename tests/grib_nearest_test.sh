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

label="grib_nearest_test"
temp=$label.temp
tempRef=$label.ref
input_grb=${data_dir}/reduced_gaussian_pressure_level.grib1

# Nearest with decoding the data values
# --------------------------------------
$EXEC ${test_dir}/grib_nearest_test $input_grb > $temp
cat > $tempRef <<EOF
((((centre)==(ecmf))&&((number)==(1)))||((step)==(6)))
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
((((centre)==(ecmf))&&((number)==(1)))||((step)==(6)))
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

# ECC-1295: regular lat/lon on ellipsoid
# ----------------------------------------
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s shapeOfTheEarth=2 $sample2 $temp
grib_check_key_equals $sample2 earthIsOblate 0
grib_check_key_equals $temp    earthIsOblate 1
${tools_dir}/grib_ls -l 0,0 $temp

# reduced lat/lon on ellipsoid
# ----------------------------------------
sample2=$ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib2.tmpl
${tools_dir}/grib_set -s shapeOfTheEarth=4 $sample2 $temp
grib_check_key_equals $sample2 earthIsOblate 0
grib_check_key_equals $temp    earthIsOblate 1
${tools_dir}/grib_ls -l 0,0 $temp

# regular gaussian on ellipsoid
# ----------------------------------------
sample2=$ECCODES_SAMPLES_PATH/regular_gg_pl_grib2.tmpl
${tools_dir}/grib_set -s shapeOfTheEarth=5 $sample2 $temp
grib_check_key_equals $sample2 earthIsOblate 0
grib_check_key_equals $temp    earthIsOblate 1
${tools_dir}/grib_ls -l 0,0 $temp

# reduced gaussian on ellipsoid
# ----------------------------------------
sample2=$ECCODES_SAMPLES_PATH/reduced_gg_pl_48_grib2.tmpl
${tools_dir}/grib_set -s shapeOfTheEarth=5 $sample2 $temp
grib_check_key_equals $sample2 earthIsOblate 0
grib_check_key_equals $temp    earthIsOblate 1
${tools_dir}/grib_ls -l 0,0 $temp


# Clean up
rm -f $temp $tempRef

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

label="grib_complex_spatial_differencing_test"
temp=${label}".grib.tmp"
temp1=${label}".1.tmp"
temp2=${label}".2.tmp"
tempLog=${label}.log


# Test meta-data
res=`${tools_dir}/grib_get -p decimalScaleFactor,bitsPerValue ${data_dir}/gfs.c255.grib2`
[ "$res" = "1 20" ]

# ECC-523
# ---------
infile=${data_dir}/gfs.complex.mvmu.grib2
grib_check_key_equals $infile 'missingValueManagementUsed,bitmapPresent' '1 0'
grib_check_key_equals $infile 'numberOfMissing,numberOfMissingValues' '556901 556901'


# ECC-511
# ----------
# GRIB2 message from NCEP/GFS with grid_complex_spatial_differencing and
# missingValueManagementUsed. No bitmap but missing values embedded in data
input=${data_dir}/gfs.complex.mvmu.grib2
stats=`${tools_dir}/grib_get -F%.2f -p max,min,avg $input`
[ "$stats" = "2.81 0.00 0.30" ]

# ECC-1926
# ----------
# grid_complex_spatial_differencing with bpv=0
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -rs packingType=grid_complex_spatial_differencing $input $temp1
grib_check_key_equals $temp1 packingType,isConstant 'grid_complex_spatial_differencing 1'
stats1=`${tools_dir}/grib_get -M -F%.0f -n statistics $input`
stats2=`${tools_dir}/grib_get -M -F%.0f -n statistics $temp1`
[ "$stats1" = "$stats2" ]
# From constant grid_complex_spatial_differencing to grib_simple
${tools_dir}/grib_set -rs packingType=grid_simple $temp1 $temp2
grib_check_key_equals $temp2 packingType,isConstant 'grid_simple 1'
${tools_dir}/grib_compare -b totalLength,section5Length,section7Length,dataRepresentationTemplateNumber $temp2 $temp1


# ECC-2060
# ------------
# grib_complex_spatial_differencing with and without bitmap
check_complex_packing() {
  input="$1"
  alg="$2"
  order="$3"

  args="packingType=$alg"
  if [ "$order" -ne 0 ]; then
    args="$args,orderOfSpatialDifferencing=$order"
  fi
  ${tools_dir}/grib_set -s $args $input $temp1 

  grib_check_key_equals $temp1 packingType "$alg"
  stats1=`${tools_dir}/grib_get -M -F%.0f -n statistics $input`
  stats2=`${tools_dir}/grib_get -M -F%.0f -n statistics $temp1`
  [ "$stats1" = "$stats2" ]
  ${tools_dir}/grib_set -rs packingType=grid_simple $temp1 $temp2
  grib_check_key_equals $temp2 packingType "grid_simple"
  ${tools_dir}/grib_compare -R codedValues=0.00001 \
    -b binaryScaleFactor,totalLength,section5Length,section7Length,dataRepresentationTemplateNumber $temp2 $temp1
}

# no bitmap
check_complex_packing ${data_dir}/regular_latlon_surface.grib2 grid_complex_spatial_differencing 2
check_complex_packing ${data_dir}/regular_latlon_surface.grib2 grid_complex_spatial_differencing 1
check_complex_packing ${data_dir}/regular_latlon_surface.grib2 grid_complex 0

# with bitmap
check_complex_packing ${data_dir}/reduced_latlon_surface.grib2 grid_complex_spatial_differencing 2
check_complex_packing ${data_dir}/reduced_latlon_surface.grib2 grid_complex_spatial_differencing 1
check_complex_packing ${data_dir}/reduced_latlon_surface.grib2 grid_complex 0


# Error conditions
# -----------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
set +e
${tools_dir}/grib_set -s orderOfSpatialDifferencing=4,packingType=grid_complex_spatial_differencing $input $temp 2>$tempLog
status=$?
set -e
[ $status -ne 0 ]
grep -q "unsupported .* orderOfSpatialDifferencing" $tempLog


# Clean up
rm -f $temp $temp1 $temp2 $tempLog

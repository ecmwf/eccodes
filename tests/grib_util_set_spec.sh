#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
#test_dir="valgrind --error-exitcode=1 "$test_dir

# --------------------------------------------------
# Regular Lat/Lon Grid
# --------------------------------------------------
infile=../data/latlon.grib
outfile=out.grib_util_set_spec.grib
tempOut=temp.grib_util_set_spec.grib
grib_util_set_spec=${test_dir}/grib_util_set_spec

rm -f $outfile

# GRIB1 with local definition for MARS. Convert to edition2 and remove local def
$grib_util_set_spec -e 2 -r $infile $outfile > /dev/null

res=`${tools_dir}/grib_get -p edition,section2Used,Ni,Nj,numberOfValues,bitsPerValue $outfile`
[ "$res" = "2 0 17 14 238 24" ]

# Check output file geometry
${tools_dir}/grib_get_data $outfile > /dev/null

# Remove the local definition from input
${tools_dir}/grib_set -s deleteLocalDefinition=1 $infile $tempOut
$grib_util_set_spec $tempOut $outfile > /dev/null

# Add another grib1 local definition (which is not in grib2)
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=5 $infile $tempOut
infile=$tempOut
$grib_util_set_spec -r -e 2 $tempOut $outfile > /dev/null
res=`${tools_dir}/grib_get -p edition,section2Used $outfile`
[ "$res" = "2 0" ]

# GRIB2 input with local definition
infile=../data/regular_latlon_surface.grib2
$grib_util_set_spec -r $infile $outfile > /dev/null
grib_check_key_equals $outfile section2Used 0
# GRIB2 input without local definition
infile=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
$grib_util_set_spec $infile $outfile > /dev/null
grib_check_key_equals $outfile section2Used 0

# Convert to edition2 and use JPEG for packing
# Don't complain due to unbound variable
set +u
if [ x"$HAVE_JPEG" != "x" ]; then
  if [ $HAVE_JPEG -eq 1 ]; then
    infile=../data/latlon.grib
    $grib_util_set_spec -e 2 -p grid_jpeg $infile $outfile > /dev/null
    res=`${tools_dir}/grib_get -p edition,section2Used,packingType $outfile`
    [ "$res" = "2 1 grid_jpeg" ]
  fi
fi
set -u

# --------------------------------------------------
# Reduced Gaussian Grid N=32 second order packing
# --------------------------------------------------
infile=../data/reduced_gaussian_model_level.grib2
outfile=out.grib_util_set_spec.grib
rm -f $outfile

stats_old=`${tools_dir}/grib_get -F%.2f -p min,max $infile`
[ "$stats_old" = "160.25 224.45" ]

$grib_util_set_spec -p grid_second_order $infile $outfile

# Check output file. Values are scaled up by 1.1
grib_check_key_equals $outfile packingType grid_second_order
stats_new=`${tools_dir}/grib_get -F%.2f -p min,max $outfile`
[ "$stats_new" = "176.28 246.90" ]

${tools_dir}/grib_get_data $outfile > /dev/null
CHECK_TOOL="${tools_dir}/grib_check_gaussian_grid"
if [ -x $CHECK_TOOL ]; then
  $CHECK_TOOL $outfile
fi

### Constant field N=32
###########################################
infile=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
rm -f $outfile

$grib_util_set_spec $infile $outfile
grib_check_key_equals $outfile "packingType,const" "grid_simple 1"
${tools_dir}/grib_get_data $outfile > /dev/null


### Clean up
rm -f $outfile $tempOut

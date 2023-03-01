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
#test_dir="valgrind --error-exitcode=1 "$test_dir

# --------------------------------------------------
# Regular Lat/Lon Grid
# --------------------------------------------------
infile=${data_dir}/latlon.grib  # edition 1
outfile=out.grib_util_set_spec.grib
tempOut=temp.grib_util_set_spec.grib
grib_util_set_spec=${test_dir}/grib_util_set_spec

rm -f $outfile

# GRIB1 with local definition for MARS. Convert to edition2 and remove local def
$EXEC $grib_util_set_spec -e 2 -r $infile $outfile > /dev/null

res=`${tools_dir}/grib_get -p edition,section2Used,Ni,Nj,numberOfValues,bitsPerValue $outfile`
[ "$res" = "2 0 17 14 238 24" ]

# Check output file geometry
${tools_dir}/grib_get_data $outfile > /dev/null

# Remove the local definition from input
${tools_dir}/grib_set -s deleteLocalDefinition=1 $infile $tempOut
$EXEC $grib_util_set_spec $tempOut $outfile > /dev/null

# Add another grib1 local definition (which is not in grib2)
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=5 $infile $tempOut
infile=$tempOut
$EXEC $grib_util_set_spec -r -e 2 $tempOut $outfile > /dev/null
res=`${tools_dir}/grib_get -p edition,section2Used $outfile`
[ "$res" = "2 0" ]

# GRIB2 input with local definition
infile=${data_dir}/regular_latlon_surface.grib2
$EXEC $grib_util_set_spec -r $infile $outfile > /dev/null
grib_check_key_equals $outfile section2Used 0
# GRIB2 input without local definition
infile=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
$EXEC $grib_util_set_spec $infile $outfile > /dev/null
grib_check_key_equals $outfile section2Used 0

# Convert to edition2 and use JPEG for packing
if [ $HAVE_JPEG -eq 1 ]; then
    infile=${data_dir}/latlon.grib
    $EXEC $grib_util_set_spec -e 2 -p grid_jpeg $infile $outfile > /dev/null
    res=`${tools_dir}/grib_get -p edition,section2Used,packingType $outfile`
    [ "$res" = "2 1 grid_jpeg" ]
fi

# CCSDS for packing and different editions
if [ $HAVE_AEC -eq 1 ]; then
    infile=${data_dir}/sample.grib2
    $EXEC $grib_util_set_spec -p grid_ccsds $infile $outfile
    grib_check_key_equals $outfile packingType grid_ccsds

    infile=${data_dir}/latlon.grib #grib1
    $EXEC $grib_util_set_spec -e 2 -p grid_ccsds $infile $outfile > /dev/null
    res=`${tools_dir}/grib_get -p edition,section2Used,packingType $outfile`
    [ "$res" = "2 1 grid_ccsds" ]

    # If we don't convert, then should leave it as grid_simple (No CCSDS in grib1)
    $EXEC $grib_util_set_spec -p grid_ccsds $infile $outfile
    grib_check_key_equals $outfile packingType grid_simple
fi

# --------------------------------------------------
# Reduced Gaussian Grid N=32 second order packing
# --------------------------------------------------
# The gaussian tests intentionally cause an error so need to stop it failing
unset ECCODES_FAIL_IF_LOG_MESSAGE

infile=${data_dir}/reduced_gaussian_model_level.grib2
outfile=out.grib_util_set_spec.grib
rm -f $outfile

stats_old=`${tools_dir}/grib_get -F%.2f -p min,max $infile`
[ "$stats_old" = "160.25 224.45" ]

$EXEC $grib_util_set_spec -p grid_second_order $infile $outfile

# Check output file. Values are scaled up by 1.1
grib_check_key_equals $outfile packingType grid_second_order
stats_new=`${tools_dir}/grib_get -F%.2f -p min,max $outfile`
[ "$stats_new" = "4.84 246.90" ]

${tools_dir}/grib_get_data $outfile > /dev/null
CHECK_TOOL="${tools_dir}/grib_check_gaussian_grid"
if [ -x $CHECK_TOOL ]; then
  $CHECK_TOOL $outfile
fi

### Constant field N=32
# ---------------------------
infile=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
rm -f $outfile

$EXEC $grib_util_set_spec $infile $outfile
grib_check_key_equals $outfile "packingType,const" "grid_simple 0"
${tools_dir}/grib_get_data $outfile > /dev/null


# CCSDS input
# ---------------------------
if [ $HAVE_AEC -eq 1 ]; then
  infile=${data_dir}/ccsds.grib2
  $EXEC $grib_util_set_spec $infile $outfile
  grib_check_key_equals $outfile packingType grid_ccsds

  $EXEC $grib_util_set_spec -p grid_simple $infile $outfile
  grib_check_key_equals $outfile packingType grid_simple

  $EXEC $grib_util_set_spec -p grid_second_order $infile $outfile
  grib_check_key_equals $outfile packingType grid_second_order
fi

# Second order input/output
# ---------------------------
${tools_dir}/grib_set -r -s packingType=grid_second_order ${data_dir}/sample.grib2 $tempOut
grib_check_key_equals $tempOut packingType grid_second_order
$EXEC $grib_util_set_spec $tempOut $outfile
grib_check_key_equals $outfile packingType grid_second_order

$EXEC $grib_util_set_spec -p grid_second_order ${data_dir}/simple.grib $outfile
grib_check_key_equals $outfile packingType grid_second_order


# Check DEBUG output
# ---------------------------
export ECCODES_DEBUG=-1
$EXEC  $grib_util_set_spec ${data_dir}/sample.grib2 $outfile > $tempOut 2>&1
grep -q "ECCODES DEBUG grib_util:" $tempOut


### Clean up
rm -f $outfile $tempOut
rm -f error.data

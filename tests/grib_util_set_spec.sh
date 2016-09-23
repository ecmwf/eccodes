#!/bin/sh
# Copyright 2005-2016 ECMWF.
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
rm -f $outfile

# GRIB1 with local definition for MARS
${test_dir}grib_util_set_spec $infile $outfile > /dev/null

res=`${tools_dir}grib_get -p edition,section2Used,Ni,Nj,numberOfValues,bitsPerValue $outfile`
[ "$res" = "2 0 17 14 238 24" ]

# Check output file geometry
${tools_dir}grib_get_data $outfile > /dev/null

# Remove the local definition from input
${tools_dir}grib_set -s deleteLocalDefinition=1 $infile $tempOut
${test_dir}grib_util_set_spec $tempOut $outfile > /dev/null

# Add another grib1 local definition (which is not in grib2)
${tools_dir}grib_set -s setLocalDefinition=1,localDefinitionNumber=5 $infile $tempOut
infile=$tempOut
${test_dir}grib_util_set_spec $tempOut $outfile > /dev/null
res=`${tools_dir}grib_get -p edition,section2Used $outfile`
[ "$res" = "2 0" ]

# GRIB2 input with local definition
infile=../data/regular_latlon_surface.grib2
${test_dir}grib_util_set_spec $infile $outfile > /dev/null
grib_check_key_equals $outfile section2Used 0
# GRIB2 input without local definition
infile=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${test_dir}grib_util_set_spec $infile $outfile > /dev/null
grib_check_key_equals $outfile section2Used 0

# --------------------------------------------------
# Reduced Gaussian Grid N=32 second order packing
# --------------------------------------------------
infile=../data/reduced_gaussian_model_level.grib2
outfile=out.grib_util_set_spec.grib
rm -f $outfile

stats_old=`${tools_dir}grib_get -F%.2f -p min,max $infile`
[ "$stats_old" = "160.25 224.45" ]

${test_dir}grib_util_set_spec $infile $outfile

# Check output file. Values are scaled up by 1.1
grib_check_key_equals $outfile packingType grid_second_order
stats_new=`${tools_dir}grib_get -F%.2f -p min,max $outfile`
[ "$stats_new" = "176.28 246.90" ]

${tools_dir}grib_get_data $outfile > /dev/null
CHECK_TOOL="${tools_dir}grib_check_gaussian_grid"
if [ -x $CHECK_TOOL ]; then
  $CHECK_TOOL $outfile
fi

### Constant field N=32
###########################################
infile=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
rm -f $outfile

${test_dir}grib_util_set_spec $infile $outfile
grib_check_key_equals $outfile "packingType,const" "grid_simple 1"
${tools_dir}grib_get_data $outfile > /dev/null


### Clean up
rm -f $outfile $tempOut

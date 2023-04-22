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

label="grib_ccsds_test"
REDIRECT=/dev/null

BLACKLIST="totalLength,section5Length,section7Length,dataRepresentationTemplateNumber"

infile=${data_dir}/ccsds.grib2
outfile1=temp.$label.1
outfile2=temp.$label.2

rm -f $outfile1 $outfile2

# Convert a non-constant grib2 file (Note: not using -r)
grib2_sample=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
${tools_dir}/grib_set -s packingType=grid_ccsds $grib2_sample $outfile1
${tools_dir}/grib_compare -c data:n $grib2_sample $outfile1


# ECC-1263
# ---------
grib2_sample=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
${tools_dir}/grib_set -s packingType=grid_ccsds $grib2_sample $outfile1
${tools_dir}/grib_set -d1 $outfile1 $outfile2
# $outfile2 is now a ccsds constant field
grib_check_key_equals $grib2_sample packingType,const "grid_simple 0"
grib_check_key_equals $outfile2     packingType,const "grid_ccsds 1"
grib_check_key_equals $outfile2     accuracy 0
rm -f $outfile1 $outfile2

# ECC-1388: Tiny values
# ---------------------
tempFilt=temp.$label.filter
IFS_SAMPLES_ROOT=${proj_dir}/ifs_samples
IFS_SAMPLE_SIMPLE=$IFS_SAMPLES_ROOT/grib1_mlgrib2/gg_ml.tmpl
IFS_SAMPLE_CCSDS=$IFS_SAMPLES_ROOT/grib1_mlgrib2_ccsds/gg_ml.tmpl
echo 'set bitsPerValue=24; set values={ 1.00000007851413483E-25, 1.00000007851413529E-25 }; write;' > $tempFilt
${tools_dir}/grib_filter -o $outfile1 $tempFilt $IFS_SAMPLE_SIMPLE
${tools_dir}/grib_filter -o $outfile2 $tempFilt $IFS_SAMPLE_CCSDS
${tools_dir}/grib_compare -c data:n $outfile1 $outfile2
rm -f $tempFilt

# ECC-1388: Constant field variant
# --------------------------------
echo 'set bitsPerValue=24; set values={ 1.00000007851413483E-25, 1.00000007851413483E-25 }; write;' > $tempFilt
${tools_dir}/grib_filter -o $outfile1 $tempFilt $IFS_SAMPLE_SIMPLE
${tools_dir}/grib_filter -o $outfile2 $tempFilt $IFS_SAMPLE_CCSDS
# Must compare the referenceValue with 0 tolerance (override the referenceValueError key)
${tools_dir}/grib_compare -R referenceValue=0 -c  referenceValue $outfile1 $outfile2
rm -f $tempFilt


# ECC-1387
# ---------
echo 'set values={6, 6, 6};write;' | ${tools_dir}/grib_filter -o $outfile2 - ${data_dir}/ccsds.grib2
grib_check_key_equals $outfile2 referenceValue,bitsPerValue '6 0'

# Change packingType
# ------------------
${tools_dir}/grib_set -r -s packingType=grid_simple $infile $outfile1
${tools_dir}/grib_compare -b $BLACKLIST $outfile1 $infile > $REDIRECT

${tools_dir}/grib_set -r -s packingType=grid_ccsds $outfile1 $outfile2
${tools_dir}/grib_compare -b $BLACKLIST $outfile1 $outfile2 > $REDIRECT

templateNumber=`${tools_dir}/grib_get -p dataRepresentationTemplateNumber $outfile2`
if [ $templateNumber -ne 42 ]; then
  echo "dataRepresentationTemplateNumber=$templateNumber. Should be 42!"
  exit 1
fi

rm -f $outfile1 $outfile2

infile=${data_dir}/reduced_latlon_surface.grib2
outfile1=$infile.tmp_ccsds.1
outfile2=$infile.tmp_ccsds.2

${tools_dir}/grib_set -r -s packingType=grid_ccsds $infile $outfile1
${tools_dir}/grib_compare -b $BLACKLIST $infile $outfile1 > $REDIRECT
${tools_dir}/grib_set -r -s packingType=grid_simple $outfile1 $outfile2
${tools_dir}/grib_compare -b $BLACKLIST $outfile2 $outfile1  > $REDIRECT

res1=`${tools_dir}/grib_get '-F%1.2f' -p min,max,avg $infile`
res2=`${tools_dir}/grib_get '-F%1.2f' -p min,max,avg $outfile1`
res3=`${tools_dir}/grib_get '-F%1.2f' -p min,max,avg $outfile2`
[ "$res1" = "$res2" ]
[ "$res1" = "$res3" ]

rm -f $outfile1 $outfile2

# ECC-297: Basic support
# --------------------------------------
infile=${data_dir}/tigge_ecmwf.grib2
outfile1=$infile.tmp_ccsds.1
outfile2=$infile.tmp_ccsds.2

${tools_dir}/grib_set -r -s bitsPerValue=17 $infile $outfile1
${tools_dir}/grib_set -r -s packingType=grid_ccsds $outfile1 $outfile2
${tools_dir}/grib_compare -c data:n $outfile1 $outfile2

# ECC-477: redundant error message during conversion
# ---------------------------------------------------
infile=${data_dir}/ccsds.grib2
grib_check_key_equals $infile accuracy 14
rm -f $outfile2
${tools_dir}/grib_set -r -s packingType=grid_simple $infile $outfile1 >$outfile2 2>&1
# there should be no error messages printed (to stdout or stderr)
[ ! -s $outfile2 ]

# ECC-1202: Check input packingType=grid_ieee
# --------------------------------------------
infile=${data_dir}/grid_ieee.grib
${tools_dir}/grib_set -r -s packingType=grid_ccsds $infile $outfile1
grib_check_key_equals $outfile1 packingType grid_ccsds
${tools_dir}/grib_set -r -s packingType=grid_simple $infile $outfile2
${tools_dir}/grib_compare -c data:n $outfile1 $outfile2

# 64bit IEEE - We can only do max. 32 with CCSDS
infile=${data_dir}/reduced_gaussian_model_level.grib2
${tools_dir}/grib_set -r -s packingType=grid_ieee,precision=2  $infile $outfile1
grib_check_key_equals $outfile1 packingType,accuracy 'grid_ieee 64'
${tools_dir}/grib_set -r -s packingType=grid_ccsds $outfile1 $outfile2
grib_check_key_equals $outfile2 packingType,accuracy 'grid_ccsds 32'


# ECC-1352: Additional tests for bitsPerValue
# --------------------------------------------
infile=${data_dir}/sample.grib2
${tools_dir}/grib_set -r -s setBitsPerValue=16,packingType=grid_ccsds $infile $outfile1
${tools_dir}/grib_set -r -s setBitsPerValue=24,packingType=grid_ccsds $infile $outfile2
grib_check_key_equals $outfile1 packingType grid_ccsds
grib_check_key_equals $outfile2 packingType grid_ccsds
${tools_dir}/grib_compare -b $BLACKLIST  $infile   $outfile1
${tools_dir}/grib_compare -c data:n      $outfile1 $outfile2


# ECC-1362
# ---------
infile=${data_dir}/ccsds_szip.grib2
res=`${tools_dir}/grib_get '-F%.3f' -p min,max,avg $infile`
[ "$res" = "-180.000 180.000 -0.044" ]

# Conversion from JPEG to CCSDS (binaryScaleFactor=0, decimalScaleFactor!=0)
# -------------------------------------------------------------------------
if [ $HAVE_JPEG -eq 1 ]; then
  ${tools_dir}/grib_copy -w count=2 ${data_dir}/v.grib2 $outfile1
  grib_check_key_equals $outfile1 packingType,bitsPerValue,binaryScaleFactor,decimalScaleFactor 'grid_jpeg 10 0 1'
  ${tools_dir}/grib_set -r -s packingType=grid_ccsds $outfile1 $outfile2
  ${tools_dir}/grib_compare -c data:n $outfile1 $outfile2
fi

# Clean up
rm -f $outfile1 $outfile2

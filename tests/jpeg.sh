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

REDIRECT=/dev/null

# First check if jpeg decode/encode feature is enabled
skip_test=0
src_config=${src_dir}/config.h
if [ -f ${src_config} ]; then
  set +e
  grep '#undef HAVE_JPEG' ${src_config} >/dev/null
  status=$?
  set -e
  if [ $status -eq 0 ]; then
    # Found the string so feature is disabled
    skip_test=1
  fi
fi
if [ $skip_test -eq 1 ]; then
  echo "JPEG feature was not enabled. Skipping this test."
  exit 0
fi
echo "JPEG feature was enabled."

BLACKLIST="totalLength,section5Length,section7Length,dataRepresentationTemplateNumber,typeOfPacking"

infile=${data_dir}/jpeg.grib2
outfile1=$infile.tmp_jpeg.1
outfile2=$infile.tmp_jpeg.2

rm -f $outfile1 $outfile2 || true

${tools_dir}grib_set -s packingType=grid_simple $infile $outfile1
${tools_dir}grib_compare -P -b $BLACKLIST,typeOfCompressionUsed,targetCompressionRatio $infile $outfile1 > $REDIRECT
${tools_dir}grib_set -s packingType=grid_jpeg $outfile1 $outfile2
${tools_dir}grib_compare -P -b $BLACKLIST $outfile1 $outfile2 > $REDIRECT

templateNumber=`${tools_dir}grib_get -p dataRepresentationTemplateNumber $outfile2`

if [ $templateNumber -ne 40 ] 
then 
  echo dataRepresentationTemplateNumber=$templateNumber
  exit 1
fi

rm -f $outfile1 $outfile2 || true

infile=${data_dir}/reduced_latlon_surface.grib2
outfile1=$infile.tmp_jpeg.1
outfile2=$infile.tmp_jpeg.2
${tools_dir}grib_set -s packingType=grid_jpeg $infile $outfile1
${tools_dir}grib_compare -P -b $BLACKLIST $infile $outfile1 > $REDIRECT
${tools_dir}grib_set -s packingType=grid_simple $outfile1 $outfile2
${tools_dir}grib_compare -P -b $BLACKLIST,typeOfCompressionUsed,targetCompressionRatio $outfile1 $outfile2  > $REDIRECT

res1=`${tools_dir}grib_get '-F%1.2f' -p min,max,avg $infile`
res2=`${tools_dir}grib_get '-F%1.2f' -p min,max,avg $outfile1`
res3=`${tools_dir}grib_get '-F%1.2f' -p min,max,avg $outfile2`
[ "$res1" = "$res2" ]
[ "$res1" = "$res3" ]

# GRIB-564 nearest 4 neighbours with JPEG packing
res=`${tools_dir}grib_get -l 0,50 $outfile1`
[ "$res" = "2.47244 2.47244 2.5115 2.51931 " ]

rm -f $outfile1 $outfile2 || true

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

REDIRECT=/dev/null

# Disable if autotools being used
src_config=${src_dir}/config.h
if [ -f ${src_config} ]; then
  exit 0
fi

BLACKLIST="totalLength,section5Length,section7Length,dataRepresentationTemplateNumber"

infile=${data_dir}/ccsds.grib2
outfile1=$infile.tmp_ccsds.1
outfile2=$infile.tmp_ccsds.2

rm -f $outfile1 $outfile2
${tools_dir}/grib_set -f -r -s packingType=grid_simple $infile $outfile1
${tools_dir}/grib_compare -P -b $BLACKLIST $outfile1 $infile > $REDIRECT

${tools_dir}/grib_set -r -s packingType=grid_ccsds $outfile1 $outfile2
${tools_dir}/grib_compare -P -b $BLACKLIST $outfile1 $outfile2 > $REDIRECT

templateNumber=`${tools_dir}/grib_get -p dataRepresentationTemplateNumber $outfile2`

if [ $templateNumber -ne 42 ]
then
  echo dataRepresentationTemplateNumber=$templateNumber
  exit 1
fi

rm -f $outfile1 $outfile2

infile=${data_dir}/reduced_latlon_surface.grib2
outfile1=$infile.tmp_ccsds.1
outfile2=$infile.tmp_ccsds.2

${tools_dir}/grib_set -r -s packingType=grid_ccsds $infile $outfile1
${tools_dir}/grib_compare -P -b $BLACKLIST $infile $outfile1 > $REDIRECT
${tools_dir}/grib_set -f -r -s packingType=grid_simple $outfile1 $outfile2
${tools_dir}/grib_compare -P -b $BLACKLIST $outfile2 $outfile1  > $REDIRECT

res1=`${tools_dir}/grib_get '-F%1.2f' -p min,max,avg $infile`
res2=`${tools_dir}/grib_get '-F%1.2f' -p min,max,avg $outfile1`
res3=`${tools_dir}/grib_get '-F%1.2f' -p min,max,avg $outfile2`
[ "$res1" = "$res2" ]
[ "$res1" = "$res3" ]

rm -f $outfile1 $outfile2

# ECC-297
infile=${data_dir}/tigge_ecmwf.grib2
outfile1=$infile.tmp_ccsds.1
outfile2=$infile.tmp_ccsds.2

${tools_dir}/grib_set -r -s bitsPerValue=17 $infile $outfile1
${tools_dir}/grib_set -r -s packingType=grid_ccsds $outfile1 $outfile2
${tools_dir}/grib_compare -P -c data:n $outfile1 $outfile2

# ECC-477: redundant error message during conversion
infile=${data_dir}/ccsds.grib2
rm -f $outfile2
${tools_dir}/grib_set -f -r -s packingType=grid_simple $infile $outfile1 >$outfile2 2>&1
# there should be no error messages printed (to stdout or stderr)
[ ! -s $outfile2 ]

rm -f $outfile1 $outfile2

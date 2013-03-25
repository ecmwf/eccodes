#!/bin/sh
# Copyright 2005-2012 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#


. ./include.sh

REDIRECT=/dev/null

infile=${data_dir}/jpeg.grib2

rm -f $infile.1 $infile.2 || true

${tools_dir}grib_set -s packingType=grid_simple $infile $infile.1
${tools_dir}grib_compare -P -b totalLength,section5Length,section7Length,dataRepresentationTemplateNumber,typeOfPacking,typeOfCompressionUsed,targetCompressionRatio $infile $infile.1 > $REDIRECT
${tools_dir}grib_set -s packingType=grid_jpeg $infile.1 $infile.2
${tools_dir}grib_compare -P -b totalLength,section5Length,section7Length,dataRepresentationTemplateNumber,typeOfPacking $infile.1 $infile.2 > $REDIRECT

templateNumber=`${tools_dir}grib_get -p dataRepresentationTemplateNumber $infile.2`

if [ $templateNumber -ne 40 ] 
then 
  echo dataRepresentationTemplateNumber=$templateNumber
  exit 1
fi

rm -f $infile.1 $infile.2 || true

infile=${data_dir}/reduced_latlon_surface.grib2

${tools_dir}grib_set -s packingType=grid_jpeg $infile $infile.1
${tools_dir}grib_compare -P -b totalLength,section5Length,section7Length,dataRepresentationTemplateNumber,typeOfPacking $infile $infile.1 > $REDIRECT
${tools_dir}grib_set -s packingType=grid_simple $infile.1 $infile.2
${tools_dir}grib_compare -P -b totalLength,section5Length,section7Length,dataRepresentationTemplateNumber,typeOfPacking,typeOfCompressionUsed,targetCompressionRatio $infile.1 $infile.2  > $REDIRECT

rm -f $infile.1 $infile.2 || true

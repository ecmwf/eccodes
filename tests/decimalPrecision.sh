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

files="regular_latlon_surface.grib2 \
       regular_latlon_surface.grib1"

for file in $files; do
  infile=${data_dir}/$file
  outfile1=${infile}_decimalPrecision_1
  outfile2=${infile}_decimalPrecision_2
  ${tools_dir}/grib_set -r -s decimalScaleFactor=1,bitsPerValue=0 $infile $outfile1
  ${tools_dir}/grib_compare -P -c data:n $infile $outfile1 > $REDIRECT

  ${tools_dir}/grib_set -s changeDecimalPrecision=1 $infile $outfile2
  ${tools_dir}/grib_compare -P -c data:n $infile $outfile2 > $REDIRECT
  ${tools_dir}/grib_compare $outfile1 $outfile2
  rm -f $outfile1 $outfile2
done

# ECC-458: spectral_complex packing
temp=temp.grib_decimalPrecision.grib
infile=${data_dir}/spectral_complex.grib1
# Catch errors re negative values
export ECCODES_FAIL_IF_LOG_MESSAGE=1
${tools_dir}/grib_set -r -s decimalScaleFactor=0 $infile $temp
${tools_dir}/grib_set -r -s decimalScaleFactor=1 $infile $temp

rm -f $temp

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

label="grib_complex_test"
temp=${label}".grib.tmp"
temp1=${label}".1.tmp"
temp2=${label}".2.tmp"

files="sample.grib2"

for file in $files; do
  infile=${data_dir}/$file
  ${tools_dir}/grib_set -r -s packingType=grid_complex $infile $temp

  grib_check_key_equals $temp packingType grid_complex

  ${tools_dir}/grib_get '-F%.2f' -p min,max,avg $infile > $temp1
  ${tools_dir}/grib_get '-F%.2f' -p min,max,avg $temp   > $temp2
  diff $temp1 $temp2

  rm -f $temp $temp1 $temp2
done

# ECC-536
infile=${data_dir}/gfs.complex.mvmu.grib2
if [ $HAVE_AEC -eq 1 ]; then
  for pt in grid_simple grid_ccsds; do
    ${tools_dir}/grib_set -r -s packingType=$pt $infile $temp
    stats1=`${tools_dir}/grib_get -M -F%.1f -n statistics $infile`
    stats2=`${tools_dir}/grib_get -M -F%.1f -n statistics $temp`
    [ "$stats1" = "$stats2" ]
    grib_check_key_equals $temp bitmapPresent,packingType "1 $pt"
    rm -f $temp
  done
fi

infile=${data_dir}/gfs.c255.grib2
grib_check_key_equals $infile 'missingValueManagementUsed,bitmapPresent' '0 1'
grib_check_key_equals $infile 'numberOfMissing,numberOfMissingValues' '7665 7665'

# Clean up
rm -f $temp $temp1 $temp2

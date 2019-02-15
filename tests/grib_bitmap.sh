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

grib1=${data_dir}/regular_latlon_surface.grib1
grib2=${data_dir}/regular_latlon_surface.grib2
infile=${data_dir}/reduced_gaussian_model_level.grib1
outfile=${data_dir}/with_bitmap.grib1
outfile1=${data_dir}/without_bitmap.grib1
temp1=out.bmp.grib1
temp2=out.bmp.grib2
tempData1=out.bmp.grib1.data
tempData2=out.bmp.grib2.data
tempRules=bitmap.rules

rm -f $outfile

${tools_dir}/grib_set -s bitmapPresent=1 $infile $outfile >$REDIRECT

${tools_dir}/grib_dump -O $infile  | grep -v FILE > $infile.dump
${tools_dir}/grib_dump -O $outfile | grep -v FILE  > $outfile.dump

diff $outfile.dump ${data_dir}/bitmap.diff
diff $infile.dump ${data_dir}/no_bitmap.diff

rm -f $infile.dump $outfile.dump || true

${tools_dir}/grib_set -s bitmapPresent=0 $outfile $outfile1 >$REDIRECT

${tools_dir}/grib_dump -O $outfile1 | grep -v FILE > $outfile1.dump
${tools_dir}/grib_dump -O $outfile  | grep -v FILE> $outfile.dump

diff $outfile1.dump ${data_dir}/no_bitmap.diff

rm -f $outfile1 $outfile1.dump $outfile $outfile.dump || true

cat > $tempRules<<EOF
set bitmapPresent=1;
set missingValue=1111;
set Ni=6;
set Nj=2;
set values={1,2,3,4,5,6,7,1111,1111,8,9,10};
write ;
EOF

${tools_dir}/grib_filter -o $temp1 $tempRules $grib1 
${tools_dir}/grib_filter -o $temp2 $tempRules $grib2

${tools_dir}/grib_get_data -m missing $temp1 > $tempData1
${tools_dir}/grib_get_data -m missing $temp2 > $tempData2

diff $tempData1 $tempData2

# ECC-511: grid_complex_spatial_differencing
# To convert to simple packing, must pass in bitmapPresent=1
# -------------------------------------------
infile=${data_dir}/gfs.complex.mvmu.grib2
${tools_dir}/grib_set -r -s bitmapPresent=1,packingType=grid_simple $infile $temp2
grib_check_key_equals $temp2 bitmapPresent,numberOfMissing,numberOfValues,numberOfPoints "1 556901 481339 1038240"

# Clean up
rm -f  $tempData1 $tempData2 $temp1 $temp2 $tempRules

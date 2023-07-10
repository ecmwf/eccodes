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
tempRef=grib_bitmap.ref

rm -f $outfile

# Add a bitmap
# -------------
${tools_dir}/grib_set -s bitmapPresent=1 $infile $outfile >$REDIRECT

${tools_dir}/grib_dump $infile  | grep -v FILE > $infile.dump
${tools_dir}/grib_dump $outfile | grep -v FILE > $outfile.dump

grib_check_key_equals $outfile section1Flags,section3Length '192 772'

if [ $ECCODES_ON_WINDOWS -eq 0 ]; then
    # There are some minute floating point differences on Windows
    # so the diff would not work there
    diff $outfile.dump ${data_dir}/bitmap.diff
    diff $infile.dump ${data_dir}/no_bitmap.diff
fi

${tools_dir}/grib_dump -O -p bitmap $outfile | grep -v FILE > $outfile.dump
cat > $tempRef <<EOF
963-1728  bitmap = 766 {
   ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, 
   ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, 
   ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, 
   ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, 
   ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, 
   ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, ff, 
   ff, ff, ff, ff
   ... 665 more values
} # g1bitmap bitmap 
EOF
diff $tempRef $outfile.dump

rm -f $infile.dump $outfile.dump $tempRef


# Remove the bitmap
# -----------------
${tools_dir}/grib_set -s bitmapPresent=0 $outfile $outfile1 >$REDIRECT

${tools_dir}/grib_dump $outfile1 | grep -v FILE > $outfile1.dump
${tools_dir}/grib_dump $outfile  | grep -v FILE > $outfile.dump

if [ $ECCODES_ON_WINDOWS -eq 0 ]; then
    diff $outfile1.dump ${data_dir}/no_bitmap.diff
fi

rm -f $outfile1 $outfile1.dump $outfile $outfile.dump


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
rm -f $tempData1 $tempData2


# ECC-1233: Allow printing of 'byte' keys e.g., bitmap, section paddings
# -----------------------------------------------------------------------
cat > $tempRules<<EOF
 print " bitmap=[bitmap]";
 print " padding_local1_1=[padding_local1_1]";
EOF
${tools_dir}/grib_filter $tempRules $temp1 > $tempData1
cat $tempData1
cat > $tempRef <<EOF
 bitmap=fe70
 padding_local1_1=00
EOF
diff $tempRef $tempData1
rm -f  $tempData1 $temp1 $tempRules $tempRef

# ECC-511: grid_complex_spatial_differencing
# To convert to simple packing, must pass in bitmapPresent=1
# ----------------------------------------------------------
infile=${data_dir}/gfs.complex.mvmu.grib2
tempSimple=temp.grib_bitmap.simple.grib
${tools_dir}/grib_set -r -s bitmapPresent=1,packingType=grid_simple $infile $tempSimple
grib_check_key_equals $tempSimple bitmapPresent,numberOfMissing,numberOfValues,numberOfPoints "1 556901 481339 1038240"
stats=`${tools_dir}/grib_get -F%.2f -p max,min,avg $tempSimple`
[ "$stats" = "2.81 0.00 0.30" ]

# Repack
${tools_dir}/grib_copy -r $infile $temp2
${tools_dir}/grib_compare -c data:n $infile $temp2
grib_check_key_equals $temp2 bitsPerValue 9  # Note: The input file has bpv=9


# Simple to grid_complex
tempComplex=temp.grib_bitmap.complex.grib
#${tools_dir}/grib_set -r -s packingType=grid_complex  $tempSimple $tempComplex # TODO: fix re-packing 
${tools_dir}/grib_set -s packingType=grid_complex  $tempSimple $tempComplex
grib_check_key_equals $tempComplex packingType,bitmapPresent,numberOfMissing,numberOfValues,numberOfPoints "grid_complex 1 556901 481339 1038240"
stats=`${tools_dir}/grib_get -F%.2f -p max,min,avg $tempComplex`
[ "$stats" = "2.81 0.00 0.30" ]
rm -f $tempComplex

# Simple to grid_complex_spatial_differencing
tempComplexSD=temp.grib_bitmap.complexSD.grib
#${tools_dir}/grib_set -r -s packingType=grid_complex_spatial_differencing  $tempSimple $tempComplexSD # TODO: fix re-packing
${tools_dir}/grib_set -s packingType=grid_complex_spatial_differencing  $tempSimple $tempComplexSD
grib_check_key_equals $tempComplexSD packingType "grid_complex_spatial_differencing"
grib_check_key_equals $tempComplexSD bitmapPresent,numberOfMissing,numberOfValues,numberOfPoints "1 556901 481339 1038240"
stats=`${tools_dir}/grib_get -F%.2f -p max,min,avg $tempComplexSD`
[ "$stats" = "2.81 0.00 0.30" ]
rm -f $tempComplexSD
rm -f $tempSimple



# Clean up
rm -f  $tempData1 $tempData2 $temp1 $temp2 $tempRules

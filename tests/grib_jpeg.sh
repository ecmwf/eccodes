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

label="grib_jpeg_test"

REDIRECT=/dev/null

BLACKLIST="totalLength,section5Length,section7Length,dataRepresentationTemplateNumber,typeOfPacking"

do_tests()
{
    infile=${data_dir}/jpeg.grib2
    outfile1=$infile.tmp_jpeg.1
    outfile2=$infile.tmp_jpeg.2

    rm -f $outfile1 $outfile2

    # Test dump
    ${tools_dir}/grib_dump -Da $infile >/dev/null 2>&1

    grib_check_key_equals $infile accuracy 14

    ${tools_dir}/grib_set -s packingType=grid_simple $infile $outfile1
    ${tools_dir}/grib_compare -P -b $BLACKLIST,typeOfCompressionUsed,targetCompressionRatio $infile $outfile1 > $REDIRECT
    ${tools_dir}/grib_set -s packingType=grid_jpeg $outfile1 $outfile2
    ${tools_dir}/grib_compare -P -b $BLACKLIST $outfile1 $outfile2 > $REDIRECT

    templateNumber=`${tools_dir}/grib_get -p dataRepresentationTemplateNumber $outfile2`

    if [ $templateNumber -ne 40 ] 
    then 
    echo dataRepresentationTemplateNumber=$templateNumber
    exit 1
    fi

    rm -f $outfile1 $outfile2

    infile=${data_dir}/reduced_latlon_surface.grib2
    outfile1=$infile.tmp_jpeg.1
    outfile2=$infile.tmp_jpeg.2
    ${tools_dir}/grib_set -s packingType=grid_jpeg $infile $outfile1
    ${tools_dir}/grib_compare -P -b $BLACKLIST $infile $outfile1 > $REDIRECT
    ${tools_dir}/grib_set -s packingType=grid_simple $outfile1 $outfile2
    ${tools_dir}/grib_compare -P -b $BLACKLIST,typeOfCompressionUsed,targetCompressionRatio $outfile1 $outfile2  > $REDIRECT

    res1=`${tools_dir}/grib_get '-F%1.2f' -p min,max,avg $infile`
    res2=`${tools_dir}/grib_get '-F%1.2f' -p min,max,avg $outfile1`
    res3=`${tools_dir}/grib_get '-F%1.2f' -p min,max,avg $outfile2`
    [ "$res1" = "$res2" ]
    [ "$res1" = "$res3" ]

    rm -f $outfile2

    # GRIB-564 nearest 4 neighbours with JPEG packing
    res=`${tools_dir}/grib_get -l 0,50 $outfile1`
    [ "$res" = "2.47244 2.47244 2.5115 2.51931 " ]

    rm -f $outfile1

    # ECC-317: Constant JPEG field numberOfValues
    # Create a JPEG encoded GRIB message to have all constant values and one more value
    # than input GRIB message
    infile=${data_dir}/jpeg.grib2
    outfile=$infile.temp.const
    tempFilter1=temp.grib_jpeg_test1.filt
    tempFilter2=temp.grib_jpeg_test2.filt
    numberOfValuesOrig=`${tools_dir}/grib_get -p numberOfValues $infile`
    # Create a filter to print the values. This will be used to create another filter
    echo " print \"set values={[values!1',']};\";" > $tempFilter1
    echo " print \"write;\";"                      >> $tempFilter1
    # Run the filter on the input. Change the output to set all values to 1 with an additional entry
    # so the output file should have original numberOfValues+1
    ${tools_dir}/grib_filter $tempFilter1 $infile |\
       sed -e 's/[0-9][0-9]*/1/' |\
       sed -e 's/set values={1,/set values={1,1,/' > $tempFilter2
    # Apply the new filter to create the constant field JPEG file
    ${tools_dir}/grib_filter -o $outfile $tempFilter2 $infile
    [ -f $outfile ]
    numberOfValuesNew=`expr $numberOfValuesOrig + 1`
    grib_check_key_equals $outfile "numberOfValues" $numberOfValuesNew
    rm -f $tempFilter1 $tempFilter2

    rm -f $outfile $outfile2
}

# Generic test. Could use jasper or openjpeg depending on build
do_tests

# Specific test in case openjpeg is linked
set +u
# Check HAVE_LIBOPENJPEG is defined and is equal to 1
if [ "x$HAVE_LIBOPENJPEG" != x ]; then
    if [ $HAVE_LIBOPENJPEG -eq 1 ]; then
        export ECCODES_GRIB_JPEG=openjpeg
        do_tests
    fi
fi

# ECC-802
# -------
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
tempFilt=temp.$label.filt
tempGrib=temp.$label.grib
cat > $tempFilt <<EOF
  set Ni = 2;
  set Nj = 2;
  set bitsPerValue = 12;
  set packingType = 'grid_jpeg';
  set values = {-0.01, 11.99, 56.11, 98.99 };
  write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample2 2>/dev/null
grib_check_key_equals $tempGrib 'packingType,numberOfValues' 'grid_jpeg 4'
stats=`${tools_dir}/grib_get -M -F%.2f -p min,max $tempGrib`
[ "$stats" = "-0.01 98.99" ]


# Constant field
${tools_dir}/grib_set -d1 ${data_dir}/jpeg.grib2 $tempGrib
grib_check_key_equals $tempGrib isConstant 1
${tools_dir}/grib_ls -n statistics $tempGrib

ECCODES_DEBUG=-1 ${tools_dir}/grib_ls -n statistics $data_dir/jpeg.grib2
${tools_dir}/grib_set -r -s unitsBias=2,unitsFactor=3 $data_dir/jpeg.grib2 $tempGrib

# typeOfCompressionUsed and targetCompressionRatio
set +e
${tools_dir}/grib_set -r -s typeOfCompressionUsed=1,targetCompressionRatio=255 $data_dir/jpeg.grib2 $tempGrib
status=$?
set -e
[ $status -ne 0 ]

set +e
${tools_dir}/grib_set -r -s typeOfCompressionUsed=1,targetCompressionRatio=0 $data_dir/jpeg.grib2 $tempGrib
status=$?
set -e
[ $status -ne 0 ]

set +e
${tools_dir}/grib_set -r -s typeOfCompressionUsed=0,targetCompressionRatio=1 $data_dir/jpeg.grib2 $tempGrib
status=$?
set -e
[ $status -ne 0 ]

tempDump=temp.$label.jpg
ECCODES_GRIB_DUMP_JPG_FILE=$tempDump  ${tools_dir}/grib_copy -r $data_dir/jpeg.grib2 $tempGrib
[ -f $tempDump ]
rm -f $tempDump

# Check Jasper decoding when it is disabled
if [ $HAVE_LIBJASPER -eq 0 ]; then
    set +e
    ECCODES_GRIB_JPEG=jasper ${tools_dir}/grib_get -n statistics $data_dir/jpeg.grib2 > $tempDump 2>&1
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "JasPer JPEG support not enabled" $tempDump

    infile=$data_dir/sample.grib2
    set +e
    ECCODES_GRIB_JPEG=jasper ${tools_dir}/grib_set -rs packingType=grid_jpeg $infile $outfile1 > $tempDump 2>&1
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "JasPer JPEG support not enabled" $tempDump
fi

# Clean up
rm -f $tempFilt $tempGrib $tempDump

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

BLACKLIST="totalLength,section5Length,section7Length,dataRepresentationTemplateNumber,typeOfPacking"

do_tests()
{
    infile=${data_dir}/jpeg.grib2
    outfile1=$infile.tmp_jpeg.1
    outfile2=$infile.tmp_jpeg.2

    rm -f $outfile1 $outfile2

    # Test dump
    ${tools_dir}/grib_dump -Da $infile >/dev/null 2>&1

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

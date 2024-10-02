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

label="grib_statistics_test"
temp1=temp1.$label.grib
temp2=temp2.$label.grib
tempFilt=temp2.$label.filt
tempText=temp2.$label.txt

files="regular_latlon_surface.grib2 regular_latlon_surface.grib1"

for file in $files; do

# Note: When we get min,max etc for the 1st time, dirty_statistics is 1
# so the statistics accessor will decode the data values (because dirty_statistics==1)
# Once it is finished, it sets dirty_statistics to 0.
# If you get min,max again, no computation is done (because dirty_statistics==0)
# But once the data values are changed, then dirty_statistics is once again 1
cat > $tempFilt <<EOF
    set Ni=2;
    set Nj=2;
    set decimalPrecision=4;
    print "Will set values...";
    set values={2.0,2.0,2.0,2.0};
    assert(dirty_statistics == 1);
    print "values=[values]";
    print "max=[max] min=[min] average=[average]";
    assert(dirty_statistics == 0);
    print "max=[max] min=[min] average=[average]";
    print "Will set values...";
    set values={2.0,5.0,2.0,2.0};
    assert(dirty_statistics == 1);
    print "values=[values]";
    print "max=[max] min=[min] average=[average]";
    assert(dirty_statistics == 0);
EOF

${tools_dir}/grib_filter $tempFilt ${data_dir}/$file > $tempText
diff ${data_dir}/statistics.out.good $tempText

done
rm -f $tempText $tempFilt


# GRIB with no missing values but some entries = 9999
# See ECC-478
# ---------------------------------------------------
input=${data_dir}/lfpw.grib1
stats=`${tools_dir}/grib_get -w count=50 -F%.2f -n statistics $input`
[ "$stats" = "10098.00 0.00 1064.19 3066.07 2.57 4.61 0.00" ]

# Scaling values in presence of real 9999 values
${tools_dir}/grib_set -s scaleValuesBy=0.5                     $input $temp1
${tools_dir}/grib_set -s missingValue=1.0E34,scaleValuesBy=0.5 $input $temp2
${tools_dir}/grib_compare $temp1 $temp2

# Offsetting values in presence of real 9999 values
${tools_dir}/grib_set -s offsetValuesBy=0.5                     $input $temp1
${tools_dir}/grib_set -s missingValue=1.0E34,offsetValuesBy=0.5 $input $temp2
${tools_dir}/grib_compare $temp1 $temp2

# ECC-511
# GRIB2 message from NCEP/GFS with grid_complex_spatial_differencing and
# missingValueManagementUsed. No bitmap but missing values embedded in data
input=${data_dir}/gfs.complex.mvmu.grib2
stats=`${tools_dir}/grib_get -F%.2f -p max,min,avg $input`
[ "$stats" = "2.81 0.00 0.30" ]

# ECC-1926
# grid_complex_spatial_differencing with bpv=0
# Create a data section similar to the attached file dswrf-1.grib2
cat >$tempFilt<<EOF
    set packingType='grid_complex_spatial_differencing';
    set numberOfGroupsOfDataValues=0;
    set orderOfSpatialDifferencing=2;
    set primaryMissingValueSubstitute=0;
    set referenceForGroupWidths = 64;
    set numberOfBitsUsedForTheGroupWidths = 4;
    set referenceForGroupLengths = 203736800;
    set trueLengthOfLastGroup = 8;
    set numberOfBitsForScaledGroupLengths = 7;
    set numberOfOctetsExtraDescriptors = 1;
    write;
EOF
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_filter -o $temp1 $tempFilt $input
stats1=`${tools_dir}/grib_get -M -F%.0f -n statistics $input`
stats2=`${tools_dir}/grib_get -M -F%.0f -n statistics $temp1`
[ "$stats1" = "$stats2" ]

# Decode as string - Null op
cat >$tempFilt<<EOF
    print "[computeStatistics:s]";
EOF
input=$data_dir/sample.grib2
${tools_dir}/grib_filter $tempFilt $input


# Clean up
rm -f $temp1 $temp2 $tempFilt $tempText

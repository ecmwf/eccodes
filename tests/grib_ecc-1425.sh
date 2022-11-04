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

label="grib_ecc-1425_test"

tempGrib=temp.$label.grib
tempFilt=temp.${label}.filt

sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat > $tempFilt <<EOF
 set dataDate = 20220301;
 set hour = 0;
 set P1 = 17;
 set P2 = 64;
 set timeRangeIndicator = 10;
 set localDefinitionNumber = 16;
 set marsClass = 'c3';
 set marsType = 'fcmean';
 set marsStream = 'msmm';
 set verifyingMonth = 202208;
 set averagingPeriod = 6;
 set forecastMonth = 6;
 assert( marsForecastMonth == 6 );
 write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib1
grib_check_key_equals  $tempGrib "mars.fcmonth" 6

# GRIB2
# ------
cat > $tempFilt <<EOF
 set dataDate = 20220214;
 set dataTime = 18;

 set setLocalDefinition = 1;
 set localDefinitionNumber = 12;
 set marsClass = 'c3';
 set marsType = 'fcmean';
 set marsStream = 'msmm';

 set productDefinitionTemplateNumber = 8;
 set forecastTime = 3288;

 set monthOfEndOfOverallTimeInterval = 8;
 set dayOfEndOfOverallTimeInterval = 1;
 set typeOfTimeIncrement = 2;
 set indicatorOfUnitForTimeRange = 3;
 set lengthOfTimeRange = 1;
 set indicatorOfUnitForTimeIncrement = 1;
 set timeIncrement = 6;
 set typeOfStatisticalProcessing = 0;
 assert( marsForecastMonth == 5 );
 write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals  $tempGrib "mars.fcmonth" 5


rm -f $tempGrib $tempFilt

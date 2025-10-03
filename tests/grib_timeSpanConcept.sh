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

label="grib_timespan_concept_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
latestOfficial=$( ${tools_dir}/grib_get -p tablesVersionLatestOfficial $sample_grib2 )
latest_codetable_file=$ECCODES_DEFINITION_PATH/grib2/tables/$latestOfficial/4.0.table
if [ -f "$latest_codetable_file" ]; then
  pdtns=$( awk '$1 !~ /#/ && $1 < 65000 {print $1}' $latest_codetable_file )
  for pdtn in $pdtns; do
    # echo "Doing $pdtn "
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='e6';
set tablesVersion=34;
set productDefinitionTemplateNumber=$pdtn;
if (defined(lengthOfTimeRange)){
 set lengthOfTimeRange=1;
}
write;
EOF
    ${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
    timeSpan=$( ${tools_dir}/grib_get -p timeSpan $tempGrib )
    if [ "$timeSpan" = 'unknown' ]; then
      echo "productDefinitionTemplateNumber=$pdtn: timeSpan not defined"
      exit 1
    fi
  done
fi

# now we test for the seasonal products
# for those we have monthyl statistics, but timespan should be none in case of a single time loop
for STREAM in 1043 1092 1221 1224 ; do
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set stream=$STREAM;
set tablesVersion=34;
set productDefinitionTemplateNumber=8;
set numberOfTimeRanges=1;
set lengthOfTimeRange=744;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib timeSpan "none"
done

# sub-seasonal, one time loop -> timeSpan = none
for TYPE in 47 48 80 81 82 83 ; do
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set stream=1122;
set type=$TYPE;
set tablesVersion=34;
set productDefinitionTemplateNumber=8;
set numberOfTimeRanges=1;
set lengthOfTimeRange=168;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib timeSpan "none"
done

# fromstart
for IUTR in 0 1 2 ; do
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set tablesVersion=34;
set productDefinitionTemplateNumber=8;
set indicatorOfUnitForTimeRange=$IUTR;
set numberOfTimeRanges=1;
set typeOfStatisticalProcessing=1;
set forecastTime=0;
set lengthOfTimeRange=1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib timeSpan "fromstart"
done

# month
for LTR in 672 696 720 744 ; do
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set tablesVersion=34;
set productDefinitionTemplateNumber=8;
set indicatorOfUnitForTimeRange=1;
set numberOfTimeRanges=1;
set typeOfStatisticalProcessing=0;
set lengthOfTimeRange=$LTR;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib timeSpan "month"
done


# Clean up
rm -f $tempGrib

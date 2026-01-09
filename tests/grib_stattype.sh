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

label="grib_stattype_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.rules

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat >$tempFilt<<EOF
    set tablesVersion=35;
    set setLocalDefinition=1;
    set class='e6';
    set stream='stte';
    set productDefinitionTemplateNumber = 8;
    set numberOfTimeRanges = 2;
    set typeOfStatisticalProcessing = {0,6};
    set indicatorOfUnitForTimeRange = {1,1};
    set lengthOfTimeRange = {720,24};
    write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib stattype,timespan "moav 24h"

# Clean up
rm -f $tempGrib $tempFilt

cat >$tempFilt<<EOF
    set tablesVersion=35;
    set setLocalDefinition=1;
    set class='e6';
    set stream='stte';
    set productDefinitionTemplateNumber = 8;
    set numberOfTimeRanges = 3;
    set typeOfStatisticalProcessing = {0,6,2};
    set indicatorOfUnitForTimeRange = {1,1,1};
    set lengthOfTimeRange = {720,24,1};
    write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib stattype,timespan "moav_dasd 1h"

# Clean up
rm -f $tempGrib $tempFilt

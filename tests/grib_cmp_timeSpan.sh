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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-XXXX
# < Add issue summary here >
# ---------------------------------------------------------

REDIRECT=/dev/null

label="grib_cmp_statwindow_test"  # Change prod to bufr or grib etc
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# 1h
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='e6';
set tablesVersion=34;
set productDefinitionTemplateNumber=8;
set indicatorOfUnitForTimeRange=1; 
set lengthOfTimeRange=1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib statwindow "1h"

# 3h
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='e6';
set tablesVersion=34;
set productDefinitionTemplateNumber=8;
set indicatorOfUnitForTimeRange=1;
set lengthOfTimeRange=3;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib statwindow "3h"

# instantaneous / none
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='e6';
set tablesVersion=34;
set productDefinitionTemplateNumber=0;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib statwindow "none"

# since-start
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='e6';
set tablesVersion=34;
set productDefinitionTemplateNumber=8;
set typeOfStatisticalProcessing=1;
set forecastTime=0;
set indicatorOfUnitForTimeRange=1;
set lengthOfTimeRange=1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib statwindow "since-start"

# Clean up
rm -f $tempGrib $tempFilt 

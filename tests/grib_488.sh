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
# This is the test for the JIRA issue GRIB-488
# GRIB2: step calculation for typeOfTimeIncrement=1
# ---------------------------------------------------------

label="grib-488_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/reduced_gg_pl_80_grib2.tmpl

cat >$tempFilt<<EOF
    set marsClass = 'em';
    set marsType  = 'fc';
    set marsStream = 'edmm';
    set experimentVersionNumber = '1605';
    set productDefinitionTemplateNumber = 11;
    set yearOfEndOfOverallTimeInterval  = 1900;
    set monthOfEndOfOverallTimeInterval = 1;
    set dayOfEndOfOverallTimeInterval   = 1;
    set hourOfEndOfOverallTimeInterval = 0;
    set numberOfMissingInStatisticalProcess = 31;
    set typeOfStatisticalProcessing = 0;
    set typeOfTimeIncrement = 1;
    set lengthOfTimeRange = 3;
    set indicatorOfUnitForTimeIncrement = 1;
    set timeIncrement = 24;

    print "[stepRange=]   [mars.step=]";
    write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib stepRange,mars.step "0-3 3"


# Clean up
rm -f $tempGrib $tempFilt

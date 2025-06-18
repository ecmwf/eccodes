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
# This is the test for JIRA issue ECC-2100
# ECC-2100: BUFR: Handle rdbtimeDate edge case
# ---------------------------------------------------------

REDIRECT=/dev/null

label="bufr_ecc-2100_test"

tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt

sample_bufr3=$ECCODES_SAMPLES_PATH/BUFR3_local.tmpl

# Create a BUFR similar to the attached one in the JIRA issue
cat >$tempFilt<<EOF
    set localYear=2025;
    set localMonth=6;
    set localDay=1;

    set typicalYearOfCentury = 25;
    set typicalMonth = 6;
    set typicalDay = 1;
    set typicalMinute = 0;

    set rdbtime = '7efbc0';
    assert( rdbtimeDate == 20250531 );
    write;
EOF
${tools_dir}/bufr_filter -o $tempBufr $tempFilt $sample_bufr3
result=$( ${tools_dir}/bufr_get -p rdbtimeDate $tempBufr )
[ "$result" = "20250531" ]

# Clean up
rm -f $tempBufr $tempFilt

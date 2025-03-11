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
# This is the test for the JIRA issue ECC-2037
# Computed key typicalDate doesn't always update properly
# ---------------------------------------------------------

REDIRECT=/dev/null

label="bufr_ecc-2037_test"

tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

cat >$tempFilt<<EOF
    set typicalYear = 2000;
    assert( typicalDate == 20001031 );
EOF
${tools_dir}/codes_bufr_filter $tempFilt $sample_bufr4

# Edge case: typicalYear<100
cat >$tempFilt<<EOF
    set typicalYear = 23;
    assert( typicalYear2 == 2023 );
    assert( typicalDate == 20231031 );
EOF
${tools_dir}/codes_bufr_filter $tempFilt $sample_bufr4

# bufr_set
${tools_dir}/bufr_set -s typicalYear=14 $sample_bufr4 $tempBufr
result=$( ${tools_dir}/bufr_get -p typicalDate $tempBufr )
[ "$result" = "20141031" ]

${tools_dir}/bufr_set -s typicalYear=2000 $sample_bufr4 $tempBufr
result=$( ${tools_dir}/bufr_get -p typicalDate $tempBufr )
[ "$result" = "20001031" ]


# Clean up
rm -f $tempBufr $tempFilt

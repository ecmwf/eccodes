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

# ---------------------------------------------------------------------
# JIRA issue ECC-2300
# A short delayed descriptor replication factor (031000) with value 0,
# nested inside other (non-empty) delayed replications and NOT located
# at the end of its enclosing block, used to cause:
#   ECCODES ERROR: Array inputShortDelayedDescriptorReplicationFactor:
#                  dimension mismatch
# Check that all combinations of the short replication factors encode
# correctly and round-trip.
# ---------------------------------------------------------------------

label="bufr_ecc-2300_test"
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# Helper that builds a filter with the given short replication factors
make_filter()
{
    cat > $tempFilt <<EOF
set edition = 4;
set masterTableNumber = 0;
set bufrHeaderCentre = 78;
set bufrHeaderSubCentre = 0;
set dataCategory = 3;
set internationalDataSubCategory = 50;
set dataSubCategory = 14;
set masterTablesVersionNumber = 39;
set numberOfSubsets = 1;
set observedData = 1;
set compressedData = 0;
set inputExtendedDelayedDescriptorReplicationFactor = {2};
set inputDelayedDescriptorReplicationFactor = {1, 2, 1, 2};
set inputShortDelayedDescriptorReplicationFactor = {$1};
set unexpandedDescriptors = {
    111000, 031002,
        007040,
        105000, 031001,
            002153,
            101000, 031000,
                015037,
            033007,
        101000, 031001, 015036
};
set pack = 1;
write;
EOF
}

# =====================================================================
# 1. The original failing case: short factors = {0, 0}
# =====================================================================
make_filter "0, 0"
${tools_dir}/bufr_filter -o $tempBufr $tempFilt $sample_bufr4
${tools_dir}/bufr_dump -p $tempBufr > $tempOut

cat > $tempRef << EOF
delayedDescriptorReplicationFactor= {
      1, 2, 1, 2}
shortDelayedDescriptorReplicationFactor= {
      0, 0}
extendedDelayedDescriptorReplicationFactor= {2}
edition=4
masterTableNumber=0
bufrHeaderCentre=78
bufrHeaderSubCentre=0
updateSequenceNumber=0
dataCategory=3
internationalDataSubCategory=50
dataSubCategory=14
masterTablesVersionNumber=39
localTablesVersionNumber=0
typicalYear=2012
typicalMonth=10
typicalDay=31
typicalHour=0
typicalMinute=2
typicalSecond=0
numberOfSubsets=1
observedData=1
compressedData=0
unexpandedDescriptors={
      111000, 031002, 007040, 105000, 031001, 002153, 101000, 031000, 015037, 033007, 
      101000, 031001, 015036 }
#1#impactParameter=MISSING
#1#satelliteChannelCentreFrequency=MISSING
#1#percentConfidence=MISSING
#1#atmosphericRefractivity=MISSING
#2#atmosphericRefractivity=MISSING
#2#impactParameter=MISSING
#2#satelliteChannelCentreFrequency=MISSING
#2#percentConfidence=MISSING
#3#atmosphericRefractivity=MISSING
#4#atmosphericRefractivity=MISSING

EOF

diff $tempOut $tempRef

# =====================================================================
# 2. All combinations of the two short factors must encode and give the
#    right number of elements:
#      - #bendingAngle          (015037) = sum of short factors
#      - #atmosphericRefractivity(015036) = 4 (from the {1,2,1,2} factors)
# =====================================================================
check_counts()
{
    factors=$1
    expected_bending=$2
    make_filter "$factors"
    ${tools_dir}/bufr_filter -o $tempBufr $tempFilt $sample_bufr4
    ${tools_dir}/bufr_dump -p $tempBufr > $tempOut

    nb=`grep -c bendingAngle $tempOut` || true
    na=`grep -c atmosphericRefractivity $tempOut` || true
    [ "$nb" = "$expected_bending" ]
    [ "$na" = "4" ]

    # Verify the decoded short factors round-trip
    grep -A1 '^shortDelayedDescriptorReplicationFactor' $tempOut | \
        tail -1 | grep -q "$factors"
}

check_counts "0, 0" 0
check_counts "1, 1" 2
check_counts "0, 1" 1
check_counts "1, 0" 1

# Clean up
rm -f $tempBufr $tempFilt $tempOut $tempRef

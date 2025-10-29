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
# This is the test for JIRA issue ECC-2153
# Check handling of empty nested delayed replication factors in BUFR 
# ---------------------------------------------------------

REDIRECT=/dev/null

label="bufr_ecc-2153_test"
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# Set filter
cat >$tempFilt<<EOF
set numberOfSubsets = 1;
set observedData = 1;
set compressedData = 0;
set inputExtendedDelayedDescriptorReplicationFactor={2};
set inputDelayedDescriptorReplicationFactor = {3,3};
set inputShortDelayedDescriptorReplicationFactor = {1,1,0,0,0,0};
set unexpandedDescriptors = {106000, 031002, 104000, 031001, 002153, 101000, 031000, 007007 };
set pack = 1;
write;
EOF

${tools_dir}/bufr_filter -o $tempBufr $tempFilt $sample_bufr4
${tools_dir}/bufr_dump -p $tempBufr > $tempOut

# Reference
cat > $tempRef << EOF
delayedDescriptorReplicationFactor= {
      3, 3}
shortDelayedDescriptorReplicationFactor= {
      1, 1, 0, 0, 0, 0}
extendedDelayedDescriptorReplicationFactor= {2}
edition=4
masterTableNumber=0
bufrHeaderCentre=98
bufrHeaderSubCentre=0
updateSequenceNumber=0
dataCategory=1
internationalDataSubCategory=255
dataSubCategory=110
masterTablesVersionNumber=24
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
      106000, 031002, 104000, 031001, 002153, 101000, 031000, 007007 }
#1#satelliteChannelCentreFrequency=MISSING
#1#height=MISSING
#2#satelliteChannelCentreFrequency=MISSING
#2#height=MISSING
#3#satelliteChannelCentreFrequency=MISSING
#4#satelliteChannelCentreFrequency=MISSING
#5#satelliteChannelCentreFrequency=MISSING
#6#satelliteChannelCentreFrequency=MISSING

EOF

# Exit if different
diff $tempOut $tempRef

# Clean up
rm -f $tempBufr $tempFilt $tempLog $tempOut $tempRef

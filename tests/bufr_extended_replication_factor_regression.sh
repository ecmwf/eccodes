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

# -----------------------------------------------------------------
# Regression test for GitHub issue ECC-456 / eccodes issue #456:
#   inputExtendedDelayedDescriptorReplicationFactor is silently
#   ignored when set AFTER unexpandedDescriptors.
#
# Root cause: UnexpandedDescriptors::pack_long calls unpack=3
# (PROCESS_NEW_DATA) followed immediately by unpack=1
# (PROCESS_DECODE). When the input factor is set before
# unexpandedDescriptors, PROCESS_NEW_DATA picks it up and encodes
# the correct count into the buffer; PROCESS_DECODE then reads that
# same buffer and gets the right value. When the factor is set
# AFTER unexpandedDescriptors, PROCESS_NEW_DATA runs with the
# default (-1, = "no override"), so 1 replication is encoded into
# the buffer; the subsequent PROCESS_DECODE reads 1; do_decode_ is
# then reset to 0, so the user's later call to set the factor and
# re-trigger unpack has no effect.
#
# Case 1 (CORRECT order: factor BEFORE unexpandedDescriptors)
#   → should always yield the requested replication count.
# Case 2 (WRONG order: factor AFTER unexpandedDescriptors)
#   → currently yields 1 (bug). After fix it must yield the
#     requested count too.
# -----------------------------------------------------------------

label="bufr_extended_replication_factor_regression_test"
tempBufr1=temp.$label.case1.bufr
tempFilt1=temp.$label.case1.filt
tempOut1=temp.$label.case1.out
tempBufr2=temp.$label.case2.bufr
tempFilt2=temp.$label.case2.filt
tempOut2=temp.$label.case2.out

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

REQUESTED_LEVELS=50

# ------------------------------------------------------------------
# Case 1: input factor set BEFORE unexpandedDescriptors (correct order)
#         This currently works and must continue to work.
# ------------------------------------------------------------------
cat >$tempFilt1<<EOF
 set edition=4;
 set masterTableNumber=0;
 set masterTablesVersionNumber=40;
 set localTablesVersionNumber=0;
 set dataCategory=31;
 set internationalDataSubCategory=255;
 set dataSubCategory=110;
 set numberOfSubsets=1;
 set observedData=1;
 set compressedData=0;
 set inputDelayedDescriptorReplicationFactor={1};
 set inputExtendedDelayedDescriptorReplicationFactor={${REQUESTED_LEVELS}};
 set unexpandedDescriptors={315012};
 set pack=1;
 write;
EOF

${tools_dir}/codes_bufr_filter -o $tempBufr1 $tempFilt1 $sample_bufr4
${tools_dir}/bufr_dump -p $tempBufr1 > $tempOut1

# extendedDelayedDescriptorReplicationFactor must equal REQUESTED_LEVELS
grep -q "extendedDelayedDescriptorReplicationFactor= {${REQUESTED_LEVELS}}" $tempOut1

# depthBelowWaterSurface must appear REQUESTED_LEVELS times
c=$(grep -c "#[0-9]*#depthBelowWaterSurface" $tempOut1)
[ "$c" -eq "$REQUESTED_LEVELS" ]

# ------------------------------------------------------------------
# Case 2: input factor set AFTER unexpandedDescriptors (wrong order)
#         This is the bug: the factor must still be honoured.
#         Currently yields replication=1 (silently wrong).
#         After the fix, this assertion must also pass.
# ------------------------------------------------------------------
cat >$tempFilt2<<EOF
 set edition=4;
 set masterTableNumber=0;
 set masterTablesVersionNumber=40;
 set localTablesVersionNumber=0;
 set dataCategory=31;
 set internationalDataSubCategory=255;
 set dataSubCategory=110;
 set numberOfSubsets=1;
 set observedData=1;
 set compressedData=0;
 set unexpandedDescriptors={315012};
 set inputDelayedDescriptorReplicationFactor={1};
 set inputExtendedDelayedDescriptorReplicationFactor={${REQUESTED_LEVELS}};
 set pack=1;
 write;
EOF

${tools_dir}/codes_bufr_filter -o $tempBufr2 $tempFilt2 $sample_bufr4
${tools_dir}/bufr_dump -p $tempBufr2 > $tempOut2

# Both assertions must hold after the fix:
grep -q "extendedDelayedDescriptorReplicationFactor= {${REQUESTED_LEVELS}}" $tempOut2
c=$(grep -c "#[0-9]*#depthBelowWaterSurface" $tempOut2)
[ "$c" -eq "$REQUESTED_LEVELS" ]

# Clean up
rm -f $tempBufr1 $tempFilt1 $tempOut1 $tempBufr2 $tempFilt2 $tempOut2

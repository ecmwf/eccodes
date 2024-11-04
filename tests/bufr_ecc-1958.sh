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

label="bufr_ecc-1958_test"
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

#...
#infile=${data_dir}/SOME_FILE
cat >$tempFilt<<EOF
 set numberOfSubsets = 1;
 set inputExtendedDelayedDescriptorReplicationFactor = {32};
 set masterTablesVersionNumber = 41;
 set updateSequenceNumber = 0;
 set bufrHeaderSubCentre = 0;
 set unexpandedDescriptors = { 109000,31002,5074,107024,5075,8085,22161,8085,102003,2134,22161 };
 write;
EOF
${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $sample_bufr4

${tools_dir}/bufr_dump -p $tempBufr | wc -l
${tools_dir}/bufr_dump -p $tempBufr > $tempOut

# Clean up
rm -f $tempBufr $tempFilt $tempLog $tempOut $tempRef

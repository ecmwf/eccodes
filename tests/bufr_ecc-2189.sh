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
# This is the test for JIRA issue ECC-2189
# Invalid value decoded after using operators 201 and 202
# ---------------------------------------------------------

REDIRECT=/dev/null

label="bufr_ecc-2189_test"
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# By default descriptor 14044 (=channelRadiance) has scale=7 width=22
# scale changed to 7+132-128  = 11
# width changed to 22+142-128 = 36
cat >$tempFilt<<EOF
 set compressedData = 1;
 set unexpandedDescriptors = {201142, 202132, 14044};
 set channelRadiance = 0.04446902;
 set pack = 1;
 write;
EOF
${tools_dir}/bufr_filter -o $tempBufr $tempFilt $sample_bufr4
result=`${tools_dir}/bufr_get -s unpack=1 -p channelRadiance -F%.8f $tempBufr`
[ "$result" = "0.04446902" ]


cat >$tempFilt<<EOF
 set unpack = 1;
 print "[channelRadiance->scale=] [channelRadiance->width=]";
 assert ( channelRadiance->scale == 11);
 assert ( channelRadiance->width == 36);
EOF
${tools_dir}/bufr_filter $tempFilt $tempBufr


# Clean up
rm -f $tempBufr $tempFilt

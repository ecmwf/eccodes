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

label="bufr_ecc-1623_test"
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempText=temp.$label.txt
tempRef=temp.$label.ref

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

cat > $tempFilt <<EOF
 set numberOfSubsets = 2;
 set compressedData  = 0;
 set unexpandedDescriptors = { 307011 };
 set icaoLocationIndicator = { "peach", "cedar" };
 set stationType = { 2, 1 };
 set pack = 1;
 write;
EOF

${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $sample_bufr4
echo 'set unpack=1; print "[icaoLocationIndicator]";' |\
            ${tools_dir}/codes_bufr_filter - $tempBufr > $tempText

cat > $tempRef<<EOF
peach cedar
EOF

diff $tempRef $tempText

rm -f $tempBufr $tempFilt $tempRef $tempText

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

label="bufr_ecc-1290_test"
temp=temp.$label
tempRules=temp.${label}.filter
tempBufr=temp.${label}.bufr

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# Sequence 315010 will give spectralWaveDensity a bit width of 38
cat > $tempRules << EOF
 set masterTablesVersionNumber=33;
 set localTablesVersionNumber=0;
 set observedData=1;
 set compressedData=0;
 set unexpandedDescriptors={ 315010 };
 set spectralWaveDensity = 6.66;
 set pack = 1;
 write;
EOF

${tools_dir}/codes_bufr_filter -o $tempBufr $tempRules $sample_bufr4
val=`${tools_dir}/bufr_get -s unpack=1 -p spectralWaveDensity $tempBufr`
[ "$val" = "6.66" ]

rm -f $tempBufr $tempRules

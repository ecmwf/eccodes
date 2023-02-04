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

set -u

label="bufr_ecc-1347_test"
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filter

sample_bufr3=$ECCODES_SAMPLES_PATH/BUFR3.tmpl

cat > $tempFilt <<EOF
  set inputDelayedDescriptorReplicationFactor= {0};
  set inputExtendedDelayedDescriptorReplicationFactor= {1458};
  set masterTableNumber=0;
  set bufrHeaderSubCentre=0;
  set bufrHeaderCentre=98;
  set updateSequenceNumber=1;
  set dataCategory=2;
  set dataSubCategory=0;
  set masterTablesVersionNumber=27;
  set localTablesVersionNumber=0;
  set numberOfSubsets=1;
  set observedData=1;
  set compressedData=0;
  set unexpandedDescriptors={
     301128, 7007, 2102, 201133, 25065, 25066, 201000, 309052, 205011,
     5001, 6001, 7001 };
  write;
EOF

${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $sample_bufr3
${tools_dir}/bufr_dump -p $tempBufr

rm -f $tempBufr $tempFilt

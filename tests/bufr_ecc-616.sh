#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-616.
# It tests encoding a BUFR file which uses the operator 203YYY.
# ---------------------------------------------------------
label="bufr_ecc-616-test"
tempRules=temp.${label}.filter
tempOut=temp.${label}.txt
tempRef=temp.${label}.ref
tempBufr=temp.${label}.bufr

input=${data_dir}/bufr/israel_observations_2017041010.bufr

cat > $tempRules <<EOF
  set unpack=1;
  set extractSubset=23;
  set doExtractSubsets=1;
  write;
  assert(numberOfSubsets==1);
EOF

${tools_dir}/codes_bufr_filter -o $tempBufr $tempRules $input

# The unexpandedDescriptors are:
#   203014 7030 7031 203255 307080
# 7030 = heightOfStationGroundAboveMeanSeaLevel
# 7031 = heightOfBarometerAboveMeanSeaLevel
cat > $tempRules <<EOF
 set unpack=1;
 print " numSubs=[numberOfSubsets]";
 print " refVals={[inputOverriddenReferenceValues',']}";
 print " height=[heightOfStationGroundAboveMeanSeaLevel]";
 print " height->ref=[heightOfStationGroundAboveMeanSeaLevel->reference]";
EOF

${tools_dir}/codes_bufr_filter $tempRules $tempBufr > $tempOut

cat > $tempRef << EOF
 numSubs=1
 refVals={-5000,-5000}
 height=-415
 height->ref=-5000
EOF
echo "Expected output:"
cat $tempRef
diff $tempRef $tempOut

rm -f $tempRules $tempRef $tempOut $tempBufr

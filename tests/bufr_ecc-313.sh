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
# This is the test for the JIRA issue ECC-313.
# It tests decoding a BUFR file which uses the operator 203YYY.
# ---------------------------------------------------------
label="bufr_ecc-313-test"
tempRules=temp.${label}.filter
tempOut=temp.${label}.txt
tempRef=temp.${label}.ref

input=${data_dir}/bufr/israel_observations_2017041010.bufr

# Check we can dump
${tools_dir}/bufr_dump $input >/dev/null

# Check values which have their reference redefined
# These are the unexpandedDescriptors:
#   203014 7030 7031 203255 307080
# 7030 = heightOfStationGroundAboveMeanSeaLevel
# 7031 = heightOfBarometerAboveMeanSeaLevel
cat > $tempRules <<EOF
 set unpack=1;
 print "h1=[heightOfStationGroundAboveMeanSeaLevel]";
 print "h2=[heightOfBarometerAboveMeanSeaLevel]";
 print "rv=[inputOverriddenReferenceValues]";
 print "rf1=[#1#heightOfStationGroundAboveMeanSeaLevel->reference]";
EOF

${tools_dir}/codes_bufr_filter $tempRules $input > $tempOut

cat > $tempRef << EOF
h1=10 60 215 564 685 5 5 555 
375 170 279 355 110 31 -330 -200 
135 280 115 -120 22 300 -415 15 
265 25 370 335 180 405 140 -255 
5 80 5 245 475 65 770 830 
-135 820 810 770 75 365 -200 460 
490 495 -200 60 945 20 845 180 
95 400 275 350 115 30 100 330 
50 10 950 475 -185 -388 700 5 
145 10 -65 0 70 936 -200 175 
360 710
h2=11 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 280 -1e+100 -1e+100 31 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100 -1e+100 22 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 810 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -387 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100 -1e+100 -1e+100 938 -1e+100 -1e+100 
-1e+100 -1e+100
rv=-5000 -5000
rf1=-5000
EOF
echo "Expected output:"
cat $tempRef
diff $tempRef $tempOut

rm -f $tempRules $tempRef $tempOut

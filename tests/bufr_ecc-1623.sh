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

label="bufr_ecc-1623_test"
tempBufrA=temp.$label.A.bufr
tempBufrB=temp.$label.B.bufr
tempFilt=temp.$label.filt
tempText=temp.$label.txt
tempRef=temp.$label.ref

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# Test 1
# ------------------------
cat > $tempFilt <<EOF
 set numberOfSubsets = 2;
 set compressedData  = 0;
 set unexpandedDescriptors = { 307011 };
 set icaoLocationIndicator = { "peach", "cedar" };
 set stationType = { 2, 1 };
 set pack = 1;
 write;
EOF

${tools_dir}/codes_bufr_filter -o $tempBufrA $tempFilt $sample_bufr4
echo 'set unpack=1; print "[icaoLocationIndicator]";' |\
            ${tools_dir}/codes_bufr_filter - $tempBufrA > $tempText

cat > $tempRef<<EOF
peach cedar
EOF

diff $tempRef $tempText

# Encode differently
cat > $tempFilt <<EOF
 set numberOfSubsets = 2;
 set compressedData  = 0;
 set unexpandedDescriptors = { 307011 };
 set #1#icaoLocationIndicator = "peach";
 set #2#icaoLocationIndicator = "cedar";
 set stationType = { 2, 1 };
 set pack = 1;
 write;
EOF

${tools_dir}/codes_bufr_filter -o $tempBufrB $tempFilt $sample_bufr4
${tools_dir}/bufr_compare $tempBufrA $tempBufrB


# Test 2
# ------------------------
cat > $tempFilt <<EOF
 set unpack = 1;
 set stationOrSiteName = {"Opeth", "Advent", "Night", "Silent", "Water", "Nectar",
                          "Black", "Rose", "Immortal", "Bid", "You", "Farewell"};
 set pack = 1;
 write;
EOF
input=$data_dir/bufr/synop_multi_subset.bufr
${tools_dir}/codes_bufr_filter -o $tempBufrA $tempFilt $input
result=`${tools_dir}/bufr_get -s unpack=1 -p '#3#stationOrSiteName' $tempBufrA`
[ "$result" = "Night" ]

# Clean up
rm -f $tempBufrA $tempBufrB $tempFilt $tempRef $tempText

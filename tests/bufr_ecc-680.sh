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
# This is the test for the JIRA issue ECC-680:
# BUFR operator 203YYY: implement encoding
# ---------------------------------------------------------
label="bufr_ecc-680_test"

tempBufr=temp.$label.bufr
tempFilt=temp.${label}.filter
tempText=temp.${label}.txt

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# Try it without overriding the reference values
# ---------------------------------------------
cat > $tempFilt <<EOF
  set unexpandedDescriptors = { 307080 };
  set heightOfStationGroundAboveMeanSeaLevel = -415;
  set heightOfBarometerAboveMeanSeaLevel     = -417;
  set pack = 1;
  write;
EOF

# Should fail
set +e
${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $sample_bufr4 2>$tempText
status=$?
set -e
[ $status -ne 0 ]
grep -q "Value .* out of range" $tempText


# Now provide the 203YYY operator
# ---------------------------------
cat > $tempFilt <<EOF
  set inputOverriddenReferenceValues = { -5000, -5000 };
  set unexpandedDescriptors = { 203014, 7030, 7031, 203255, 307080 };
  set heightOfStationGroundAboveMeanSeaLevel = -415;
  set heightOfBarometerAboveMeanSeaLevel     = -417;
  set pack = 1;
  write;
EOF

${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $sample_bufr4

val=`${tools_dir}/bufr_get -s unpack=1 -p heightOfStationGroundAboveMeanSeaLevel $tempBufr`
[ "$val" = "-415" ]
val=`${tools_dir}/bufr_get -s unpack=1 -p heightOfBarometerAboveMeanSeaLevel $tempBufr`
[ "$val" = "-417" ]

# Add another instance but outside the 203YYY block. Should fail
# --------------------------------------------------------------
cat > $tempFilt <<EOF
  set inputOverriddenReferenceValues = { -5000 };
  set unexpandedDescriptors = { 203014, 7030, 203255, 307080, 203000, 7030 };
  set #1#heightOfStationGroundAboveMeanSeaLevel = -415;
  set #2#heightOfStationGroundAboveMeanSeaLevel = -416; # ref val not overridden
  set pack = 1;
  write;
EOF
set +e
${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $sample_bufr4 2>$tempText
status=$?
[ $status -ne 0 ]
set -e
fgrep -q "Value (-416) out of range" $tempText

# Clean up
rm -f $tempBufr $tempFilt $tempText

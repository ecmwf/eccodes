#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-673
# bufr_filter script and paths with spaces
# ---------------------------------------------------------
cd ${data_dir}/bufr
label="bufr_ecc_673_test"

tempBufr=temp.${label}.bufr
tempRules=temp.${label}.filter

# --------------------------------------------------------
# Test 1: input bufr with spaces in name
# --------------------------------------------------------
tempBufr="temp.${label}.airs 5 7.bufr"
cp airs_57.bufr "${tempBufr}"
cat > $tempRules <<EOF
 set unpack=1;
 print "[satelliteIdentifier!0]";
EOF
${tools_dir}/bufr_filter $tempRules "${tempBufr}"

# --------------------------------------------------------
# Test 2: input bufr and rules file with spaces in name
# --------------------------------------------------------
tempRules2="temp.${label} . filter"
cat > "$tempRules2" <<EOF
 set unpack=1;
 print "[satelliteIdentifier!0]";
EOF
${tools_dir}/bufr_filter "${tempRules2}" "${tempBufr}"

# Clean up
rm -rf "${tempRules}" "${tempRules2}" "${tempBufr}"

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
# This is the test for the JIRA issue ECC-433.
# It tests decoding a BUFR file which uses the operator 206YYY.
# ---------------------------------------------------------
label="bufr_ecc-433-test"
tempRules=temp.${label}.filter
tempOut=temp.${label}.txt
tempRef=temp.${label}.ref
tempBufr=temp.${label}.bufr

input=${data_dir}/bufr/b002_95.bufr

# Check we can decode the local ECMWF descriptor 021192
# which comes after the operator 206YYY
# ------------------------------------------------------------
${tools_dir}/bufr_dump $input > $tempOut
grep -q radarBackScatter $tempOut

cat > $tempRules <<EOF
 set unpack=1;
 print "[radarBackScatter]";
EOF

${tools_dir}/codes_bufr_filter $tempRules $input > $tempOut

cat > $tempRef << EOF
59 59 57 51 47 48 45 45 
45 42 44 45 41 39 39 36 
33 34 33 31 35 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100 -1e+100 35 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 -1e+100 
-1e+100 -1e+100 -1e+100
EOF

diff $tempRef $tempOut

# Now change its centre so local ECMWF tables are not found but
# dump should still succeed (we use the operator 206 widths to read data section).
# Warnings should be generated for the local descriptors
# ------------------------------------------------------------
${tools_dir}/bufr_set -s centre=255 $input $tempBufr
${tools_dir}/bufr_dump $tempBufr >/dev/null 2>$tempOut
grep -q "unable to get descriptor 021192 from table" $tempOut

rm -f $tempRules $tempRef $tempOut $tempBufr

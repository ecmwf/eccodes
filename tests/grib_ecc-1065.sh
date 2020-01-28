#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1065
# It tests the power operator
# ---------------------------------------------------------
label="grib_ecc-1065-test"
tempRules=temp.${label}.filter

cat > $tempRules <<EOF
    transient tpow = 10 ^ 2;
    print "[tpow]";
EOF

result=`${tools_dir}/grib_filter $tempRules $ECCODES_SAMPLES_PATH/GRIB2.tmpl`
[ "$result" = "100" ]

rm -f $tempRules

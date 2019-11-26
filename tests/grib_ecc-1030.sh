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
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1030
# grib_get with the ordering option '-B'
# ---------------------------------------------------------
label="grib_ecc-1030-test"
tempRef=temp.${label}.ref
tempOut=temp.${label}.out

# Test 1
# ------
input=${data_dir}/pad.grib
${tools_dir}/grib_get -p count,level -B'level:i' $input > $tempOut
cat > $tempRef <<EOF
1 0
2 0
3 0
4 1
5 1000
6 1000
EOF
diff -w $tempRef $tempOut

# Test 2
# ------
input=${data_dir}/pad.grib
${tools_dir}/grib_get -p count,level -B'level:i desc' $input > $tempOut
cat > $tempRef <<EOF
1 1000
2 1000
3 1
4 0
5 0
6 0
EOF
diff -w $tempRef $tempOut


# Clean up
rm -f $tempRef $tempOut

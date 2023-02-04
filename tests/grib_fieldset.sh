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
label="grib_fieldset_test"
temp=temp.$label.txt
tempRef=temp.$label.ref
input_grb=${data_dir}/high_level_api.grib2

$EXEC ${test_dir}/grib_fieldset 'step:i asc' $input_grb > $temp
cat > $tempRef <<EOF
Ordering by step:i asc

5 fields in the fieldset
step,date,levelType,levelType
0 20110225 sfc 151
6 20110225 sfc 151
12 20110225 sfc 151
18 20110225 sfc 151
24 20110225 sfc 151
EOF
diff $tempRef $temp


$EXEC ${test_dir}/grib_fieldset 'step:i desc' $input_grb > $temp
cat > $tempRef <<EOF
Ordering by step:i desc

5 fields in the fieldset
step,date,levelType,levelType
24 20110225 sfc 151
18 20110225 sfc 151
12 20110225 sfc 151
6 20110225 sfc 151
0 20110225 sfc 151
EOF
diff $tempRef $temp

# Clean up
rm -f $temp $tempRef

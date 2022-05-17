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

label="grib_element_test"
tempRef=temp.${label}.ref
tempText=temp.${label}.txt
tempFilt=temp.${label}.filt

input=$ECCODES_SAMPLES_PATH/reduced_gg_pl_48_grib2.tmpl

# Print the last three entries from the "pl" array
cat > $tempFilt <<EOF
    meta elemA element(pl, Nj - 3);
    meta elemB element(pl, Nj - 2);
    meta elemC element(pl, Nj - 1);
    print "elemA=[elemA], elemB=[elemB], elemC=[elemC]";
EOF
${tools_dir}/grib_filter $tempFilt $input > $tempText
echo "elemA=36, elemB=25, elemC=20" > $tempRef
diff $tempRef $tempText


# Invalid element
cat > $tempFilt <<EOF
    meta badElem element(pl, -1);
    print "[badElem]";
EOF
set +e
${tools_dir}/grib_filter $tempFilt $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid element.*Value must be between 0 and 95" $tempText


rm -f $tempRef $tempText $tempFilt

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
tempGrib=temp.${label}.grib
tempRef=temp.${label}.ref
tempText=temp.${label}.txt
tempFilt=temp.${label}.filt

input=$ECCODES_SAMPLES_PATH/reduced_gg_pl_48_grib2.tmpl

# Print the last three entries from the "pl" array
cat > $tempFilt <<EOF
    if( element(pl,0) != 20 ) {
        assert( 0 );
    }
    meta elemA element(pl, Nj - 3);
    meta elemB element(pl, Nj - 2);
    meta elemC element(pl, Nj - 1);
    meta elemZ element(pl, -1); # another way of getting the last element
    print "elemA=[elemA], elemB=[elemB], elemC=[elemC], elemZ=[elemZ]";
    set elemZ = 1099; # Set the last element, using a -ve index
EOF
${tools_dir}/grib_filter $tempFilt $input > $tempText
echo "elemA=36, elemB=25, elemC=20, elemZ=20" > $tempRef
diff $tempRef $tempText


# Invalid element indexes
cat > $tempFilt <<EOF
    meta badElem element(pl, -97);
    print "[badElem]";
EOF
set +e
${tools_dir}/grib_filter $tempFilt $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid element.*Value must be between 0 and 95" $tempText

cat > $tempFilt <<EOF
    meta badElem element(pl, 197);
    print "[badElem]";
EOF
set +e
${tools_dir}/grib_filter $tempFilt $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid element.*Value must be between 0 and 95" $tempText


# Access a double array
input=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl
cat > $tempFilt <<EOF
    meta elemZ element(values, -1);
    print "Last value as a double = [elemZ:d]";
EOF
${tools_dir}/grib_filter $tempFilt $input

cat > $tempFilt <<EOF
    meta badElem element(values, 100000);
    print "[badElem:d]";
EOF
set +e
${tools_dir}/grib_filter $tempFilt $input
status=$?
set -e
[ $status -ne 0 ]

# Encode a single double element in an array
input=$data_dir/sample.grib2
cat > $tempFilt <<EOF
    print "Before:  [min=] [max=]";
    transient avg1 = avg;
    transient max1 = max;
    meta elemN element(values, -1);
    assert( elemN < 301 );
    print "Last elem was: [elemN:d]";
    set elemN = 312; # Pass in an integer. Should call pack_double
    print "Last elem now: [elemN:d]";
    print "After:  [min=] [max=]";
    assert( elemN > 310 );
    assert ( avg > avg1 );
    assert ( max > max1 );
    write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input


# Clean up
rm -f $tempRef $tempText $tempFilt $tempGrib

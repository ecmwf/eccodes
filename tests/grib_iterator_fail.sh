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

if [ $HAVE_GEOGRAPHY -eq 0 ]; then
    exit 0
fi

label="grib_iterator_fail_test"
tempText=temp.$label.txt
tempGrib=temp.$label.grib

# Badly encoded Gaussian sub area
# --------------------------------
tempFilt=temp.${label}.filt
cat > $tempFilt <<EOF
    meta pl1 element(pl, 1);
    set pl1 = 0;
    write;
EOF
input=$data_dir/reduced_gaussian_sub_area.grib2
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input
set +e
${tools_dir}/grib_get_data $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid pl array" $tempText
rm -f $tempFilt


# ECC-1642: badly encoded regular grids
# -------------------------------------
${tools_dir}/grib_set -s Ni=33 $samp_dir/GRIB2.tmpl $tempGrib
set +e
${tools_dir}/grib_get_data $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Grid description is wrong or inconsistent" $tempText

# Bad Ni
${tools_dir}/grib_set -s Ni=MISSING $samp_dir/GRIB2.tmpl $tempGrib
set +e
${tools_dir}/grib_get_data $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Grid description is wrong or inconsistent" $tempText


set +e
${tools_dir}/grib_ls -s Ni=missing  -j -p latLonValues $data_dir/sample.grib2 > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
cat $tempText
grep -q "Key Ni cannot be 'missing' for a regular grid" $tempText
grep -q "latlonvalues: Unable to create iterator" $tempText

# ------------------------
# non-existing key
# ------------------------
input=$data_dir/sample.grib2
${tools_dir}/grib_get_data -f -p nonexistingkey $input > $tempText
grep -q "not found" $tempText


# ------------------------
# Bad options
# ------------------------
input=$data_dir/sample.grib2
set +e
${tools_dir}/grib_get_data -Lxxx $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid lats/lons format option" $tempText


# ------------------------
# Unreadable message
# ------------------------
echo GRIB > $tempGrib
set +e
${tools_dir}/grib_get_data $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
cat $tempText
grep -q "unreadable message" $tempText


# Bad -s
set +e
${tools_dir}/grib_get_data -s blah=999 $data_dir/sample.grib2 > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Key/value not found" $tempText

# Bad -p
set +e
${tools_dir}/grib_get_data -f -p values $data_dir/sample.grib2 > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Passed array is too small" $tempText


# Clean up
rm -f $tempText $tempGrib

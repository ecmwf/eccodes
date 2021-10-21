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
label="grib_mars_keys"
grib1_sample=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
temp=temp.${label}.out
types_table=$ECCODES_DEFINITION_PATH/mars/type.table
classes_table=$ECCODES_DEFINITION_PATH/mars/class.table
streams_table=$ECCODES_DEFINITION_PATH/mars/stream.table

# Check numeric codes are unique
# -------------------------------
tables="$types_table $classes_table $streams_table"
for table in $tables; do
    echo "Testing numeric codes in $table ..."
    awk '{print $1}' < $table > $temp
    count1=`sort -u $temp | wc -l`
    count2=`wc -l $temp | awk '{print $1}'`
    [ $count1 = $count2 ]
done

# Check abbreviations are unique
# -------------------------------
tables="$types_table $classes_table $streams_table"
for table in $tables; do
    echo "Testing abbreviations in $table ..."
    awk '{print $2}' < $table > $temp
    count1=`sort -u $temp | wc -l`
    count2=`wc -l $temp | awk '{print $1}'`
    [ $count1 = $count2 ]
done

# Check MARS types
# ------------------------------
# Exclude type=34 (go, Gridded observations) as it loads definitions/mars/grib.oper.go.def
# which requires the key 'N' to exist
mars_types=`awk '$1 !~ /34/ {print $1}' < $types_table`

for t in $mars_types; do
    #echo "Doing MARS type |$t|"
    ${tools_dir}/grib_set -s marsType=$t,edition=2 $grib1_sample $temp
    grib_check_key_equals $temp "mars.type:i" $t
done


rm -f $temp

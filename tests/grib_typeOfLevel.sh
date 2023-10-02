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

label="grib_typeOfLevel_test"
temp=temp.$label.txt

if [ ! -d "$ECCODES_DEFINITION_PATH" ]; then
    echo "Test $0 disabled. No definitions directory"
    exit 0
fi

# Make sure all typeOfLevel values are unique
def_file="$ECCODES_DEFINITION_PATH/grib2/typeOfLevelConcept.def"

rm -f $temp

# uniq -d outputs a single copy of each line that is repeated in the input
grep "^'" $def_file | awk -F= '{print $1}' | tr -d "' " | sort | uniq -d > $temp

if [ -s "$temp" ]; then
    # File exists and has a size greater than zero
    echo "ERROR: Duplicates found in $def_file" >&2
    cat $temp
    exit 1
else
    echo "No duplicates in $def_file"
fi

rm -f $temp

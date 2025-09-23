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

label="error_codes_test"
tempText=temp.$label.txt

# Check src dir
# -------------
src_dir=$proj_dir/src/eccodes
gribapi_file=$src_dir/grib_api.h
eccodes_file=$src_dir/eccodes.h

count1=$( awk '/define.*\t-?[0-9]/ {count++} END{print count}' $gribapi_file )
count2=$( awk '/define.*\tGRIB/    {count++} END{print count}' $eccodes_file )

if [ $count1 -ne $count2 ]; then
    echo "Error: Check counts of error codes in $gribapi_file and $eccodes_file"
    echo "count1=$count1 but count2=$count2"
    echo "These should match"
    exit 1
fi

# Make sure the codes are unique
# uniq -d outputs a single copy of each line that is repeated in the input
awk '/define.*\t-?[0-9]/ {print $3}' $gribapi_file | sort -n | uniq -d > $tempText
if [ -s "$tempText" ]; then
    # File exists and has a size greater than zero
    echo "ERROR: Duplicate error code(s) found in $gribapi_file" >&2
    cat $tempText
    exit 1
fi

# Check F90 codes too
# -------------------
f90_dir=$proj_dir/fortran
gribapi_file=$f90_dir/grib_api_constants.h
eccodes_file=$f90_dir/eccodes_constants.h
cat $eccodes_file | sed -e 's/CODES_/GRIB_/' > $tempText
diff -w $gribapi_file $tempText


# Clean up
rm -f $tempText

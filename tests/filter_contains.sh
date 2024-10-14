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

label="filter_contains_test"
tempFilt=temp.$label.txt
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# The contains functor:
#  1st argument: Key name (haystack)
#  2nd argument: string (needle)
#  3rd argument 0=case-sensitive, 1=case-insensitive
#
cat > $tempFilt <<EOF
    if (! contains(identifier, "GR", 0) )  { assert (false); }
    if (! contains(identifier, "ib", 1) )  { assert (false); }
    if (! contains(identifier, "",   1) )  { assert (false); }

    if (contains(identifier, "grb", 1) ) { assert (false); }
    if (contains(identifier, "ib",  0) ) { assert (false); }
EOF
cat $tempFilt
${tools_dir}/grib_filter $tempFilt $sample

# Clean up
rm -f $tempFilt

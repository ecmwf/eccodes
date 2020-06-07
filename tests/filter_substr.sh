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

label="filter_substr_test"
temp=temp.$label.txt
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_filter - $sample <<EOF
    transient input = "abcdefg";
    
    # String
    transient xx = substr("abcdefg", 0, 2);
    assert( xx is "ab");
    transient xx = substr("abcdefg", 6, 1);
    assert( xx is "g");
    
    # Key
    transient xx = substr(input, 0, 2);
    assert( xx is "ab");
    transient xx = substr(input, 6, 1);
    assert( xx is "g");
EOF

# Illegal cases
${tools_dir}/grib_filter - $sample 2>$temp <<EOF
    transient xx = substr("abc", -1, -1);
EOF
grep -q "Invalid substring" $temp
rm -f $temp

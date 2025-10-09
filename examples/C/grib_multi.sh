#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label=examples_c_grib_multi
tempRef=temp.$label.ref
tempText=temp.$label.txt

# Test decoding multi-field GRIB2 data
${examples_dir}/c_grib_multi > $tempText
grep -q "codes_handle_new_from_file counted 56 messages" $tempText


${examples_dir}/c_multi2 > $tempText
cat > $tempRef <<EOF
Pass 1:
File offset start = 0
0 : stepRange=0
1 : stepRange=12
2 : stepRange=24
3 : stepRange=36
Pass 2:
File offset start = 0
0 : stepRange=0
1 : stepRange=12
2 : stepRange=24
3 : stepRange=36
Pass 3:
File offset start = 0
0 : stepRange=0
1 : stepRange=12
2 : stepRange=24
3 : stepRange=36
Pass 4:
File offset start = 0
0 : stepRange=0
1 : stepRange=12
2 : stepRange=24
3 : stepRange=36
All OK
EOF
diff $tempRef $tempText

# Clean up
rm -f $tempText $tempRef

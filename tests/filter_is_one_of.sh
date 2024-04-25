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

label="filter_is_one_of_test"
tempFilt=temp.$label.filt
tempOut=temp.$label.txt

input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
grib_check_key_equals $input "shortName,paramId,typeOfLevel" "t 130 surface"

cat > $tempFilt <<EOF
  transient xx1 = is_one_of(paramId, 0, 99, 44, 130); # last one should match
  assert(xx1 == 1);
  transient xx2 = is_one_of(paramId, 666); # no match
  assert(xx2 == 0);

  transient xx3 = is_one_of(shortName, "t", "q"); # First one matches
  assert(xx3 == 1);

  transient xx4 = is_one_of(typeOfLevel, "isobar", "surface" , "nomatch");
  assert(xx4);
EOF

${tools_dir}/grib_filter $tempFilt $input

# Clean up
rm -f $tempFilt $tempOut

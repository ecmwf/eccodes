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

label="filter_size_test"
tempFilt=temp.$label.filt
tempOut=temp.$label.txt

cat > $tempFilt <<EOF
  if (size(pl) == 64) {
    print "size of pl is 64";
  }
  if (size(pl) == 128) {
    print "size of pl is 128";
  }
EOF

${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl > $tempOut
grep "size of pl is 64" $tempOut

${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/reduced_gg_pl_64_grib2.tmpl > $tempOut
grep "size of pl is 128" $tempOut

# Clean up
rm -f $tempFilt $tempOut

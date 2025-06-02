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

label="filter_is_in_list_test"
temp=temp.$label.txt
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Evaluate long
${tools_dir}/grib_filter - $sample <<EOF
  transient a_name="#";
  if (is_in_list(a_name, "boot.def")) {
    print "Found";
  }
EOF

# Evaluate string
${tools_dir}/grib_filter - $sample <<EOF
  transient a_name="#";
  transient the_cond = (is_in_list(a_name, "boot.def"));
  if (the_cond) {
    print "Found";
  }
EOF

# Illegal cases
${tools_dir}/grib_filter - $sample 2>$temp <<EOF
  transient a_name="xyz";
  if (is_in_list(a_name, "existential nihilism")) {
    print "Found";
  }
EOF
grep -q "unable to find def file" $temp


# Clean up
rm -f $temp

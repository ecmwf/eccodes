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

# Define a common label for all the tmp files
label="bufr_dump_samples_test"

temp=${label}".temp"
rm -f $temp

# Test sample BUFR files
for file in $ECCODES_SAMPLES_PATH/BUFR*.tmpl; do
  ${tools_dir}/bufr_dump -O $file >/dev/null
  ${tools_dir}/bufr_dump -d $file >/dev/null
done

# Check one specific BUFR file dump output
input=$ECCODES_SAMPLES_PATH/BUFR3.tmpl
${tools_dir}/bufr_dump -p $input > $temp
diff ${data_dir}/BUFR3.tmpl.dump.plain.ref $temp

# Code generation
input=$ECCODES_SAMPLES_PATH/BUFR4.tmpl
for lang in C python fortran filter; do
  ${tools_dir}/bufr_dump -D $lang $input >/dev/null
  ${tools_dir}/bufr_dump -E $lang $input >/dev/null
done

rm -f $temp

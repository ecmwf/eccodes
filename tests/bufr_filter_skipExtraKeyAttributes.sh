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

cd ${data_dir}/bufr

# Define a common label for all the tmp files
label="bufr_filter_skip_test"

tempFilt=temp.${label}.filter
tempRef=temp.${label}.ref
tempOut=temp.${label}.txt

input="ship_13.bufr"

# Decode fully including extra attributes
# ---------------------------------------
cat > $tempFilt <<EOF
  set unpack = 1;
  print "  windDirectionAt10M=[windDirectionAt10M]";
  print "  windDirectionAt10M->units=[windDirectionAt10M->units]";
EOF
cat $tempFilt
${tools_dir}/codes_bufr_filter -f $tempFilt $input > $tempOut
cat > $tempRef <<EOF
  windDirectionAt10M=30
  windDirectionAt10M->units=deg
EOF
diff $tempRef $tempOut



# Decode excluding extra attributes
# ---------------------------------------
cat > $tempFilt <<EOF
  set skipExtraKeyAttributes = 1;
  set unpack = 1;
  print "  windDirectionAt10M=[windDirectionAt10M]";
  print "  windDirectionAt10M->units=[windDirectionAt10M->units]";
EOF
cat $tempFilt
${tools_dir}/codes_bufr_filter -f $tempFilt $input > $tempOut
cat > $tempRef <<EOF
  windDirectionAt10M=30
  windDirectionAt10M->units=undef
EOF
diff $tempRef $tempOut


# Clean up
rm -f $tempFilt $tempRef $tempOut

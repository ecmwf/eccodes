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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1288.
# It tests decoding a BUFR file which cannot be done unless
# we extend the definitions to include extra files
# ---------------------------------------------------------
label="bufr_ecc-1288_test"

TEMP=${label}.temp
LOG=${label}.log
set -u

# Create a temporary directory which holds the tables etc
TEMP_DIR=${label}.temp-dir.$$
rm -rf $TEMP_DIR
mkdir -p $TEMP_DIR/definitions/bufr/tables/0/local/0/65535/0

# Copy the definition files needed to decode our BUFR file
def_files="${proj_dir}/tests/ECC-197/*"
for df in ${def_files}; do
  cp -f $df $TEMP_DIR/definitions/bufr/tables/0/local/0/65535/0
done
LOCAL_DEFS=`pwd`/$TEMP_DIR/definitions

input=${data_dir}/bufr/vos308014_v3_26.bufr
$EXEC ${test_dir}/bufr_ecc-1288 $input "$LOCAL_DEFS" # 2>$LOG

#grep -q "ECCODES ERROR   :  hash_array: no match for sequences=308014" $LOG

# Clean up
rm -rf $TEMP_DIR
rm -f $TEMP $LOG

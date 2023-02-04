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
# This is the test for the JIRA issue ECC-197.
# It tests decoding a BUFR file which cannot be done unless
# we extend the definitions to include extra files
# ---------------------------------------------------------
label="bufr_ecc-197_test"

input=${data_dir}/bufr/vos308014_v3_26.bufr
TEMP=${label}.temp
LOG=${label}.log
set -u
decode_bufr()
{
    infile=$1
    ${tools_dir}/codes_info
    ${tools_dir}/bufr_dump $infile > $TEMP
    # Ensure output JSON has all the expected contents
    # After calling 'wc' and 'set', $1 will be the line count
    set `wc -l $TEMP`
    [ $1 -gt 35700 ]
    grep -q encryptedShipOrMobileLandStationIdentifier $TEMP
}


# Get expanded descriptors
cat > $TEMP <<EOF
  print "expandedDescriptors=[expandedDescriptors%06d!0]";
EOF
${tools_dir}/bufr_filter -f $TEMP $input > $LOG 2>&1
grep -q "ECCODES ERROR.*no match for sequences=308014" $LOG


# Create a temporary directory which holds the tables etc
# This BUFR file uses masterTablesVersionNumber 26
TEMP_DIR=${label}.temp-dir.$$
rm -rf $TEMP_DIR
mkdir -p $TEMP_DIR/definitions/bufr/tables/0/wmo/26

# Copy the definition files needed to decode our BUFR file
def_files="${proj_dir}/tests/ECC-197/*"
for df in ${def_files}; do
  cp -f $df $TEMP_DIR/definitions/bufr/tables/0/wmo/26
done

# Point to dir containing the tables
MY_DEFS=`pwd`/$TEMP_DIR/definitions
export ECCODES_EXTRA_DEFINITION_PATH=$MY_DEFS
decode_bufr $input
unset ECCODES_EXTRA_DEFINITION_PATH

if [ $HAVE_MEMFS -eq 1 ]; then
    unset ECCODES_DEFINITION_PATH
    std_defs=`${tools_dir}/codes_info -d`
    export ECCODES_DEFINITION_PATH=$MY_DEFS:$std_defs
    decode_bufr $input
    unset ECCODES_DEFINITION_PATH
    export ECCODES_EXTRA_DEFINITION_PATH=$MY_DEFS
    decode_bufr $input
else
    # Decode by extending the ECCODES_DEFINITION_PATH
    export ECCODES_DEFINITION_PATH=$MY_DEFS:$ECCODES_DEFINITION_PATH
    decode_bufr $input
fi

rm -rf $TEMP_DIR
rm -f $TEMP $LOG

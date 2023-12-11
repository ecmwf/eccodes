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

label="bufr_ecc-1259_test"
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempOut=temp.$label.out

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# Check no crash if invalid encoding
# -------------------------------------
cat > $tempFilt <<EOF
    set masterTablesVersionNumber = 35;
    set unexpandedDescriptors = {340019};
    set compressedData = 1;  ### ouch
    set satelliteIdentifier = 66;
    set pack = 1;
    write;
EOF

# An error will be generated but suppressed (No crash). So exit code should be 0
${tools_dir}/bufr_filter -f $tempFilt $sample_bufr4 2>$tempOut

# Check the right error message came out
grep -q "ECCODES ERROR.*Error while setting key 'pack' (Encoding invalid)" $tempOut


# Now fix the order of keys and rerun
# -------------------------------------
cat > $tempFilt <<EOF
    set masterTablesVersionNumber = 35;
    set compressedData = 1;  ### fixed
    set unexpandedDescriptors = {340019};
    set satelliteIdentifier = 66;
    set pack = 1;
    write;
EOF
${tools_dir}/bufr_filter -o $tempBufr $tempFilt $sample_bufr4
${tools_dir}/bufr_dump -p $tempBufr

# Clean up
rm -f $tempFilt $tempBufr $tempOut

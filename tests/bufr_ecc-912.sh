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
# ECC-912: BUFR decoding error when operator 201 is active
# before a class 31 delayed replication factor descriptor.
# Operator 201YYY should not modify class 31 descriptors.
# ---------------------------------------------------------

label=`basename $0 | sed -e 's/\.sh/_test/'`
tempFilt=temp.$label.filt
tempOut=temp.$label.out
tempBufr=temp.$label.bufr

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

cat > $tempFilt <<EOF
    set inputExtendedDelayedDescriptorReplicationFactor= {11};

    set unexpandedDescriptors={ 201132, 202129,
    101000, 031002, 021001,
    201000, 202000};

    set #1#horizontalReflectivity=4;
    set #2#horizontalReflectivity=6;
    set #3#horizontalReflectivity=12;
    set #4#horizontalReflectivity=12;
    set #5#horizontalReflectivity=66;
    set pack = 1;
    write;
EOF

${tools_dir}/bufr_filter -o$tempBufr $tempFilt $sample_bufr4
${tools_dir}/bufr_dump -p $tempBufr > $tempOut 2>&1

grep -q "#5#horizontalReflectivity=66" $tempOut
grep -q "#6#horizontalReflectivity=MISSING" $tempOut

# Check the output does not contain ERROR
if grep -q "ERROR" $tempOut; then
    echo "ERROR: bufr_dump should not produce errors for this file"
    cat $tempOut
    exit 1
fi


# Use a filter to verify the file can be fully decoded
cat > $tempFilt <<EOF
  set unpack=1;
  print "[numberOfSubsets]";
EOF

# Use a filter to verify the file can be fully decoded
result=$(${tools_dir}/codes_bufr_filter $tempFilt $tempBufr)
# Should print 1 (number of subsets)
[ "$result" = "1" ]

rm -f $tempFilt $tempOut $tempBufr

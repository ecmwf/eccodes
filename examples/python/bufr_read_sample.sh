#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh


# Define a common label for all the tmp files
label="bufr_read_sample_test_p"

# Define tmp files
temp1=${label}".tmp1.txt"
temp2=${label}".tmp2.txt"

$PYTHON $examples_src/bufr_read_sample.py ${ECCODES_SAMPLES_PATH}/BUFR3.tmpl > $temp1
$PYTHON $examples_src/bufr_read_sample.py ${ECCODES_SAMPLES_PATH}/BUFR4.tmpl > $temp2

# Check output
kvpairs='
   unexpandedDescriptors=307080
   blockNumber=MISSING
   horizontalVisibility=MISSING
   #3#cloudType=MISSING'

for kv in $kvpairs; do
    grep -q $kv $temp1
    grep -q $kv $temp2
done

# Clean up
rm -f $temp1 $temp2

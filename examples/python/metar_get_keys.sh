#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
if [ $HAVE_MEMFS -eq 1 ]; then
   echo "$0: This test is currently disabled with MEMFS"
   exit 0
fi

#Define a common label for all the tmp files
label="metar_get_keys_test_p"

#Define tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp

REDIRECT=/dev/null

# Write the key values into a file
$PYTHON $examples_src/metar_get_keys.py 2> $REDIRECT > $fTmp

# Check the results
grep -q "message: 157" $fTmp
count=`grep -c latitude $fTmp`
[ $count -eq 126 ]
count=`grep -c dewPointTemperature $fTmp`
[ $count -eq 133 ]


# Clean up
rm -f $fTmp

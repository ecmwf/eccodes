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
label="gts_get_keys_test_p"

# Define tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp

# Write the key values into a file
$PYTHON $examples_src/gts_get_keys.py > $fTmp

# Check the results
grep -q "message: 300" $fTmp
c=`grep -c "CCCC: KWBC" $fTmp`
[ $c -eq 211 ]

#Clean up
rm -f $fTmp

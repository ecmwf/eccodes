#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh


#Define a common label for all the tmp files
label="bufr_read_tropical_cyclone_p"

#Define tmp file
fTmp=${label}.tmp.txt
#ref=${label}.tmp.ref
#rm -f $fTmp | true

#We check "asca_1391.bufr". The path is hardcoded in the example

REDIRECT=/dev/null

echo $PYTHON $examples_src
#Write the key values into a file
#$PYTHON $examples_src/bufr_read_tropical_cyclone.py >$fTmp
$PYTHON $examples_src/bufr_read_tropical_cyclone.py >$fTmp

# Check the results
grep -q "Date and time:  18 . 11 . 2015    0 : 0" $fTmp
grep -q "== Member  52" $fTmp

#cat > $ref <<EOF
#EOF
# 
#diff -w $ref $fTmp
#Clean up
#rm -f $ref
rm -f $fTmp

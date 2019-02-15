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
label="bufr_read_tropical_cyclone_f"

#Define tmp file
fTmp=${label}".tmp.txt"
rm -f $fTmp

#We check "tropical_cyclone.bufr". The path is hardcoded in the example

REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/eccodes_f_bufr_read_tropical_cyclone 2> $REDIRECT > $fTmp

# Check the output
grep -q "Date and time: 18.11.2015  0:0" $fTmp
grep -q "== Member  52" $fTmp

#Clean up
rm -f $fTmp

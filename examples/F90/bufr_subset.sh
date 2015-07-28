#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

#Define a common label for all the tmp files
label="bufr_subset_test_f"

#Prepare tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp | true

#We check "synop_multi_subset.bufr". The path is
#hardcoded in the example

REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/eccodes_f_bufr_subset 2> $REDIRECT > $fTmp

#TODO: add a proper check when subsets are properly implemented

#We compare output to the reference by ignoring the whitespaces 
#diff -w $fRef $fTmp >$REDIRECT 2> $REDIRECT

#cat $fTmp

#Clean up
rm -f $fTmp

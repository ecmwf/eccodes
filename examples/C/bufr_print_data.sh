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


#Enter data dir
cd ${data_dir}/bufr

#Define a common label for all the tmp files
label="bufr_print_data_test"

#Define tmp file
fTmp=${label}".tmp.txt"
rm -f $fTmp

#-----------------------------------------------------
# Test reading the header only from a BUFR
# file with multiple messages
#----------------------------------------------------
f="syno_multi.bufr"
fRef=$f".header.ref"
REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/bufr_print_data  ${data_dir}/bufr/$f 2> $REDIRECT > $fTmp
#diff $fRef $fRes >$REDIRECT 2> $REDIRECT

#cat  $fTmp

#Clean up
rm -f $fTmp | true

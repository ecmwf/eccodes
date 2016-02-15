#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

#Define a common label for all the tmp files
label="bufr_read_header_test_f"

#Prepare tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp | true

#-----------------------------------------------------
# Test reading the header from a BUFR
# file with multiple messages
#----------------------------------------------------

f=${data_dir}/bufr/syno_multi.bufr
fRef=${f}.header.ref

REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/eccodes_f_bufr_read_header  $f 2> $REDIRECT > $fTmp

#We compare output to the reference by ignoring the whitespaces 
diff -w $fRef $fTmp >$REDIRECT 2> $REDIRECT

#cat $fRes

#Clean up
rm -f ${fTmp} | true




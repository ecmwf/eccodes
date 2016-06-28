#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh
set -x
#Define a common label for all the tmp files
label="bufr_get_string_array_test_f"

#Prepare tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp | true

#-----------------------------------------------------
# Test get string array from a BUFR
#----------------------------------------------------

fRef=${data_dir}/bufr/get_string_array.ref

REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/eccodes_f_bufr_get_string_array > $fTmp

#We compare output to the reference by ignoring the whitespaces 
diff -w $fRef $fTmp

#cat $fRes

#Clean up
rm -f ${fTmp} | true




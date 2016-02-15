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
label="bufr_expanded_test_f"

#Prepare tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp | true

#-----------------------------------------------------
# Test reading the expanded values
#----------------------------------------------------
  
#f=${data_dir}/bufr/syno_1.bufr
REDIRECT=/dev/null

#We check "syno_1.bufr". The path is
#hardcoded in the example

#Write the values into a file
${examples_dir}/eccodes_f_bufr_expanded > $fTmp 2> $REDIRECT

#TODO: add a better check. It could be compared against the bufrdc
# references.

#Check if there is any output
[ -s ${fTmp} ]

#cat $fTmp 

#Clean up
rm -f ${fTmp}

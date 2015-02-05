#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

#set -x

#Define a common label for all the tmp files
label="bufr_expanded_test.c"

f=${data_dir}/bufr/syno_1.bufr

#Prepare tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp | true

#Reference file
ref_num=${f}.num.ref
  
REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/bufr_expanded $f > $fTmp 2> $REDIRECT 

[ `wc -l ${fTmp}` -gt 100 ]

#kdiff3 $ref_num $fTmp

# Cannot use plain diff. We need to compare FLOAT NUMBERS with a tolerance
#perl /tmp/cgr/build/grib_api/eccodes/tests/number_compare.pl $ref_num $fTmp #>$REDIRECT 2> $REDIRECT
  
  
#cat $fTmp 

#Clean up
rm -f $fTmp

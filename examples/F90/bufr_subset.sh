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
label="bufr_subset_test_f"

#Prepare tmp file
fTmp=${label}.tmp.txt
fTmp2=${label}.tmp2.txt
rm -f $fTmp

#Prepare ref file
fRef=${label}.ref
cat > $fRef <<EOF
 message:            0
   numberOfSubsets:          12
 /subsetNumber=00001/blockNumber                                                                     
  subsetNumber:           1
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00002/blockNumber                                                                     
  subsetNumber:           2
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00003/blockNumber                                                                     
  subsetNumber:           3
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00004/blockNumber                                                                     
  subsetNumber:           4
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00005/blockNumber                                                                     
  subsetNumber:           5
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00006/blockNumber                                                                     
  subsetNumber:           6
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00007/blockNumber                                                                     
  subsetNumber:           7
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00008/blockNumber                                                                     
  subsetNumber:           8
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00009/blockNumber                                                                     
  subsetNumber:           9
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00010/blockNumber                                                                     
  subsetNumber:          10
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00011/blockNumber                                                                     
  subsetNumber:          11
   blockNumber:           1
   stationNumber:         485
 /subsetNumber=00012/blockNumber                                                                     
  subsetNumber:          12
   blockNumber:           1
   stationNumber:         485
EOF

#We check "synop_multi_subset.bufr". The path is
#hardcoded in the example

REDIRECT=/dev/stdout

#Write the values into a file and compare with reference
${examples_dir}/eccodes_f_bufr_subset 2> $REDIRECT > $fTmp

# Remove blank lines
sed '/^\s*$/d' < $fTmp > $fTmp2

#We compare output to the reference by ignoring the whitespaces 
diff -w $fRef $fTmp2 >$REDIRECT 2> $REDIRECT

#Clean up
rm -f $fTmp $fTmp2 $fRef

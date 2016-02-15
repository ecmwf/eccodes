#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

#Define a common label for all the tmp files
label="bufr_subset_test_p"

#Prepare tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp | true

#Prepare ref file
fRef=${label}.ref
cat > $fRef<<EOF
message: 0
 numberOfSubsets: 12
/subsetNumber=1/blockNumber
  /subsetNumber=1/blockNumber= 1
  /subsetNumber=1/stationNumber: 27
/subsetNumber=2/blockNumber
  /subsetNumber=2/blockNumber= 1
  /subsetNumber=2/stationNumber: 84
/subsetNumber=3/blockNumber
  /subsetNumber=3/blockNumber= 1
  /subsetNumber=3/stationNumber: 270
/subsetNumber=4/blockNumber
  /subsetNumber=4/blockNumber= 1
  /subsetNumber=4/stationNumber: 272
/subsetNumber=5/blockNumber
  /subsetNumber=5/blockNumber= 1
  /subsetNumber=5/stationNumber: 308
/subsetNumber=6/blockNumber
  /subsetNumber=6/blockNumber= 1
  /subsetNumber=6/stationNumber: 371
/subsetNumber=7/blockNumber
  /subsetNumber=7/blockNumber= 1
  /subsetNumber=7/stationNumber: 381
/subsetNumber=8/blockNumber
  /subsetNumber=8/blockNumber= 1
  /subsetNumber=8/stationNumber: 382
/subsetNumber=9/blockNumber
  /subsetNumber=9/blockNumber= 1
  /subsetNumber=9/stationNumber: 387
/subsetNumber=10/blockNumber
  /subsetNumber=10/blockNumber= 1
  /subsetNumber=10/stationNumber: 413
/subsetNumber=11/blockNumber
  /subsetNumber=11/blockNumber= 1
  /subsetNumber=11/stationNumber: 464
/subsetNumber=12/blockNumber
  /subsetNumber=12/blockNumber= 1
  /subsetNumber=12/stationNumber: 485
EOF

#We check "synop_multi_subset.bufr". The path is
#hardcoded in the example

REDIRECT=/dev/stdout

#
$PYTHON $examples_src/bufr_subset.py 2> $REDIRECT > $fTmp

diff $fTmp $fRef

#Clean up
rm -f $fTmp $fRef


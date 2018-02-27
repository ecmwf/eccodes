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
label="bufr_subset_test_c"

#Prepare tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp | true

#Prepare ref file
fRef=${label}.ref
cat > $fRef <<EOF
message: 0
  numberOfSubsets: 12
  subsetNumber=1  blockNumber=1  stationNumber=27  stationOrSiteName="TROMSO-HOLT"  airTemperature=276.45
  subsetNumber=2  blockNumber=1  stationNumber=84  stationOrSiteName="PASVIK"  airTemperature=266.55
  subsetNumber=3  blockNumber=1  stationNumber=270  stationOrSiteName="KVITHAMAR"  airTemperature=275.25
  subsetNumber=4  blockNumber=1  stationNumber=272  stationOrSiteName="FROSTA"  airTemperature=276.25
  subsetNumber=5  blockNumber=1  stationNumber=308  stationOrSiteName="FURUNESET"  airTemperature=276.85
  subsetNumber=6  blockNumber=1  stationNumber=371  stationOrSiteName="LOKEN I VOLBU"  airTemperature=265.35
  subsetNumber=7  blockNumber=1  stationNumber=381  stationOrSiteName="APELSVOLL"  airTemperature=270.15
  subsetNumber=8  blockNumber=1  stationNumber=382  stationOrSiteName="KISE"  airTemperature=270.45
  subsetNumber=9  blockNumber=1  stationNumber=387  stationOrSiteName="FAVANG"  airTemperature=267.55
  subsetNumber=10  blockNumber=1  stationNumber=413  stationOrSiteName="SAERHEIM"  airTemperature=277.45
  subsetNumber=11  blockNumber=1  stationNumber=464  stationOrSiteName="LANDVIK"  airTemperature=275.95
  subsetNumber=12  blockNumber=1  stationNumber=485  stationOrSiteName="SANDE-GALLEBERG"  airTemperature=275.45
EOF

#We check "synop_multi_subset.bufr". The path is
#hardcoded in the example

REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/c_bufr_subset 2> $REDIRECT > $fTmp

#We compare output to the reference by ignoring the whitespaces 
diff -w $fRef $fTmp >$REDIRECT 2> $REDIRECT

#Clean up
rm -f $fTmp $fRef

#!/bin/sh

set -e
dir=$PWD

toolsDir=../tools/
dataDir=../data/
cd $dataDir

echo "-# The default behaviour for bufr_compare without any option is to perform a bit by bit comparison of the two messages. If the messages 
are found to be bitwise different then bufr_compare switches to a \"key based\" mode to find out which coded keys are different.
 To see how bufr_compare works we first set key <i>bufrHeaderSubCentre</i>=1 in the file syno_1.bufr\\n
\\verbatim 
>bufr_set -s bufrHeaderSubCentre=1 data/bufr/syno_1.bufr out.bufr
\\endverbatim\\n"

rm -f out.bufr | true
$toolsDir/bufr_set -s bufrHeaderSubCentre=1  bufr/syno_1.bufr  out.bufr
echo "Then we can compare the two fields with bufr_compare.\\n"
echo "\\verbatim 
>bufr_compare data/bufr/syno_1.bufr out.bufr"
set +e
$toolsDir/bufr_compare bufr/syno_1.bufr  out.bufr
set -e
echo "\\endverbatim
In the output we can see that the only \"coded\" key with different values in the two messages is <i>bufrHeaderSubCentre</i> .
The comparison can be forced to be successful by listing the keys with different values in the -b option."

echo "\\verbatim 
>bufr_compare -b bufrHeaderSubCentre data/bufr/syno_1.bufr out.bufr"
set +e
$toolsDir/bufr -b bufrHeaderSubCentre bufr/syno_1.bufr out.bufr 
set -e
echo "\\endverbatim\\n"




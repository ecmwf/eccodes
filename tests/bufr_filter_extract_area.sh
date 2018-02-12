#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

set -x
. ./include.sh

cd ${data_dir}/bufr

#Define a common label for all the tmp files
label="bufr_filter_extract_area"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp bufr file
fBufrTmp=${label}".bufr.tmp"

#Define filter rules file
fRules=${label}.filter

#-----------------------------------------------------------
# Test: Area extraction
#-----------------------------------------------------------
cat > $fRules <<EOF
 transient originalNumberOfSubsets = numberOfSubsets;
 set unpack=1;
 set extractAreaNorthLatitude=52.5;
 set extractAreaSouthLatitude=51.1;
 set extractAreaWestLongitude=155.2;
 set extractAreaEastLongitude=160.5;
 set extractAreaLongitudeRank=1;
 set doExtractArea=1;
 write;
 print "extracted [numberOfSubsets] of [originalNumberOfSubsets] subsets";
 assert(numberOfSubsets == extractedAreaNumberOfSubsets);
EOF

inputBufr="amsa_55.bufr"
outputBufr=${label}.${inputBufr}.out
outputFilt=${label}.txt
outputRef=${label}.ref
rm -f $outputFilt

echo "Test: Area extraction" >> $fLog
echo "file: $outputBufr" >> $fLog

${tools_dir}/codes_bufr_filter -o $outputBufr $fRules $inputBufr > $outputFilt
[ -f $outputBufr ]

cat > $fRules <<EOF
set unpack=1;
print "latitude=[latitude!15]";
print "===========";
print "longitude=[longitude!15]";
print "===========";
print "second=[second!15]";
print "===========";
print "scanLineNumber=[scanLineNumber!15]";
print "===========";
print "fieldOfViewNumber=[fieldOfViewNumber!15]";
print "===========";
EOF
${tools_dir}/codes_bufr_filter $fRules $inputBufr $outputBufr  >> $outputFilt

cat > $outputRef <<EOF
extracted 14 of 128 subsets
latitude=49.2875 49.8194 50.2426 50.5906 50.8842 51.1371 51.3586 51.5555 51.7329 51.8945 52.0434 52.1819 52.3118 52.4349 52.5523 
52.6652 52.7746 52.8814 52.9863 53.0901 53.1934 53.2968 53.4009 53.5061 53.6127 53.7209 53.8303 53.9401 54.0476 54.1472 
48.8597 49.3864 49.8055 50.1502 50.4409 50.6913 50.9107 51.1058 51.2815 51.4417 51.5892 51.7265 51.8553 51.9773 52.0937 
52.2057 52.3142 52.4202 52.5244 52.6275 52.7302 52.833 52.9365 53.0413 53.1475 53.2555 53.365 53.475 53.5832 53.6842 
48.4308 48.9524 49.3675 49.7089 49.9969 50.2449 50.4623 50.6555 50.8297 50.9884 51.1346 51.2706 51.3983 51.5193 51.6347 
51.7459 51.8536 51.9588 52.0622 52.1647 52.2667 52.369 52.472 52.5763 52.6823 52.7901 52.8996 53.0099 53.1188 53.2211 
48.0008 48.5175 48.9286 49.2668 49.5521 49.7978 50.0132 50.2047 50.3772 50.5346 50.6795 50.8143 50.941 51.0609 51.1755 
51.2857 51.3926 51.4971 51.5998 51.7016 51.803 51.9048 52.0073 52.1112 52.2169 52.3245 52.434 52.5447 52.6543 52.758 
47.5699 48.0817 48.489 48.824 49.1066 49.3501 49.5635 49.7533
===========
longitude=167.2984 165.6325 164.2137 162.9735 161.8672 160.8636 159.9403 159.0802 158.27 157.4991 156.7586 156.041 155.3397 154.6487 153.9623 
153.2751 152.5816 151.8761 151.1526 150.4041 149.623 148.8001 147.9245 146.9823 145.9563 144.8234 143.5526 142.1003 140.4019 138.3564 
166.9615 165.3065 163.8977 162.6667 161.569 160.5735 159.6579 158.8051 158.002 157.2381 156.5043 155.7934 155.0987 154.4143 153.7346 
153.0542 152.3676 151.6693 150.9531 150.2123 149.4394 148.6252 147.7588 146.8268 145.8119 144.6914 143.4347 141.9985 140.3191 138.2966 
166.6306 164.9864 163.5874 162.3655 161.2762 160.2887 159.3806 158.5351 157.739 156.9817 156.2546 155.5503 154.862 154.1841 153.5109 
152.8371 152.1573 151.4659 150.7569 150.0237 149.2586 148.4529 147.5956 146.6734 145.6693 144.5609 143.3178 141.8974 140.2365 138.2363 
166.3055 164.672 163.2827 162.0697 160.9887 160.009 159.1083 158.2699 157.4806 156.73 156.0094 155.3113 154.6295 153.9579 153.291 
152.6237 151.9504 151.2658 150.5638 149.838 149.0807 148.2831 147.4347 146.5221 145.5286 144.4319 143.2021 141.7969 140.154 138.1756 
165.9862 164.3631 162.9834 161.7792 160.7063 159.7343 158.8409 158.0094
===========
second=23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 
23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 
31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 
31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 
39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 
39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 
47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 
47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 
55.54 55.54 55.54 55.54 55.54 55.54 55.54 55.54
===========
scanLineNumber=266 266 266 266 266 266 266 266 266 266 266 266 266 266 266 
266 266 266 266 266 266 266 266 266 266 266 266 266 266 266 
267 267 267 267 267 267 267 267 267 267 267 267 267 267 267 
267 267 267 267 267 267 267 267 267 267 267 267 267 267 267 
268 268 268 268 268 268 268 268 268 268 268 268 268 268 268 
268 268 268 268 268 268 268 268 268 268 268 268 268 268 268 
269 269 269 269 269 269 269 269 269 269 269 269 269 269 269 
269 269 269 269 269 269 269 269 269 269 269 269 269 269 269 
270 270 270 270 270 270 270 270
===========
fieldOfViewNumber=1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 
16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 
16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 
16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 
16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 
1 2 3 4 5 6 7 8
===========
latitude=51.3586 51.5555 51.7329 51.8945 52.0434 52.1819 52.3118 51.1058 51.2815 51.4417 51.5892 51.7265 51.1346 51.2706
===========
longitude=159.9403 159.0802 158.27 157.4991 156.7586 156.041 155.3397 158.8051 158.002 157.2381 156.5043 155.7934 156.2546 155.5503
===========
second=23.54 23.54 23.54 23.54 23.54 23.54 23.54 31.54 31.54 31.54 31.54 31.54 39.54 39.54
===========
scanLineNumber=266 266 266 266 266 266 266 267 267 267 267 267 268 268
===========
fieldOfViewNumber=7 8 9 10 11 12 13 8 9 10 11 12 11 12
===========
EOF

diff $outputRef $outputFilt

# Uncompressed message
# ---------------------
inputBufr="delayed_repl_01.bufr"
outputBufr=${label}.${inputBufr}.out
cat > $fRules <<EOF
 transient originalNumberOfSubsets = numberOfSubsets;
 set unpack=1;
 set extractAreaNorthLatitude = -21.0;
 set extractAreaSouthLatitude = -25.0;
 set extractAreaWestLongitude = 136;
 set extractAreaEastLongitude = 154;
 set extractAreaLongitudeRank=1;
 set doExtractArea=1;
 write;
 print "extracted [numberOfSubsets] of [originalNumberOfSubsets] subsets";
 assert(3 == extractedAreaNumberOfSubsets);
EOF

${tools_dir}/codes_bufr_filter -o $outputBufr $fRules $inputBufr
ns=`${tools_dir}/bufr_get -p numberOfSubsets $outputBufr`
[ $ns -eq 3 ]

rm -f $outputRef $outputFilt $outputBufr $fLog $fRules

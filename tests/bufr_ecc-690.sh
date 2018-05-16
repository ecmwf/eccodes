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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-690.
# Decoding NOAA GOES message percent confidence values
# which do not have a bitmap.
# ---------------------------------------------------------
label="bufr_ecc-690-test"
tempRules=temp.${label}.filter
tempOut=temp.${label}.txt
tempBufr=temp.${label}.bufr
tempRef=temp.${label}.ref

input=${data_dir}/bufr/goes16_nm.bufr

cat > $tempRules <<EOF
  set unpack=1;
  print "#1#percentConfidence=[#1#percentConfidence!30]";
  print "#2#percentConfidence=[#2#percentConfidence!30]";
  print "#3#percentConfidence=[#3#percentConfidence!30]";
  print "#4#percentConfidence=[#4#percentConfidence!30]";
EOF

export ECCODES_BUFR_QUALITY_WITHOUT_BITMAP=1
${tools_dir}/codes_bufr_filter $tempRules $input > $tempOut

cat > $tempRef << EOF
#1#percentConfidence=2147483647
#2#percentConfidence=63 82 90 62 72 97 93 67 79 84 75 64 79 89 79 83 84 83 77 65 77 76 63 61 79 92 93 90 76 87 
76 72 72 84 64 88 91 71 95 83 82 67 78 83 67 82 88 94 61 68 71 98 66 68 66 67 82 63 95 83 
89 61 88 95 62 76 61 64 84 80 67 82 66 61 65 71 85 82 84 73 65 81 84 91 81 70 65 72 60 77 
62 63 76 62 87 83 91 60 89 84 82 69 94 61 66 84 83 61 82 73 61 96 94 68 75 86 90 63 96 66 
85 83 87 99 99 69 64 97 69 81 92 70 67 68 84 80 78 82 67 67 88 83 79 89 86 87 87 79 93 90 
65 66 64 78 82 88 73 65 79 75 83 66 75 75 80 71 97 82 84 84 76 62 64 76 75 84 67 65 90 63 
61 71 62 74 63 86 88 72 61 74 87 87 84 82 73 67 84 85 68 92 80 84 74 82 73 86 70 73 71 84 
70 69 61 70 75 99 86 76 95 89 63 88 92 87 70 78 77 64 71 72 85 71 65 72 78 69 79 62 79 72 
68 88 66 74 66 92 80 92 79 72 78 68 69 89 85 85 87 74 69 74 64 91 85 84 82 74 80 69 62 82 
79 91 75 87 65 79 62 86 76 81 83 62 85 73 84 87 80 82 89 84 92 78 84 62 71 78 69 66 94 65 
68 88 90 61 81 82 75 83 64 71 77 81 65 65 79 69 79 88 90 77 68 71 82 68 65 68 83 80 69 68 
65 70 83 82 97 96 66 91 83 83 95 94 99 96 95 96 95 94 95 98 94 96 94 63 72 81 73 76 94 88 
94 94 93 90 80 94 95 89 89 86 85 75 61 82 72 65 88 64 82 65 80 73 83 87 71 74 64 69 81 78 
73 65 75 78 73 64 64 93 90 64 90 88 85 73 69 72 89 74 89 73 72 95 91 86 95 94 83 93 98 75 
94 88 72 73 79 91 69 79 82 100 100 96 77 64 96 91 92 98 60 78 63 96 71 98 79 90 60 71 90 75 
61 66 82 61 60 93 89 64 78 98 99
#3#percentConfidence=2147483647
#4#percentConfidence=63 37 42 63 60 71 70 60 43 3 62 63 58 68 61 46 3 1 10 48 56 47 60 61 61 64 68 69 60 58 
50 62 65 66 49 66 70 58 75 55 55 54 65 56 49 69 67 69 43 53 55 75 74 46 71 53 57 49 73 74 
74 44 70 75 48 55 71 56 69 56 51 50 67 49 50 53 57 54 46 57 51 42 57 75 58 56 73 53 33 55 
73 74 41 41 59 56 75 57 64 57 57 58 77 67 62 65 65 40 53 52 50 63 76 74 50 56 75 63 63 53 
60 53 61 77 77 59 76 76 64 69 74 57 60 67 68 66 69 74 59 66 71 64 67 68 65 65 72 65 72 71 
65 59 48 70 62 68 68 48 55 59 53 60 52 57 62 59 75 70 53 53 61 49 58 57 60 71 44 58 69 70 
70 47 52 69 49 59 78 71 73 29 76 77 75 69 65 52 74 66 47 62 30 63 49 52 46 44 51 66 69 73 
54 45 46 57 60 69 77 68 75 74 68 76 73 70 67 57 58 41 57 60 65 60 70 72 65 58 49 62 74 55 
68 63 63 68 68 70 67 72 69 59 66 26 27 67 67 67 70 67 26 64 58 70 67 66 71 61 59 58 51 66 
71 73 66 69 71 70 68 73 68 72 61 54 65 63 71 70 55 61 73 73 76 53 61 63 64 61 78 76 78 64 
78 71 71 73 68 72 72 71 58 67 55 57 56 75 56 73 66 65 64 58 54 49 54 57 58 55 63 56 54 50 
47 51 54 53 71 71 53 68 52 52 67 67 72 68 66 68 66 59 61 63 59 60 58 53 57 50 45 59 58 49 
59 60 59 50 47 60 59 67 67 64 67 59 57 63 58 53 63 49 55 49 51 53 60 65 46 60 58 55 75 74 
68 66 66 70 69 53 52 70 68 65 67 68 72 68 67 67 69 62 69 73 73 71 70 69 71 70 69 73 75 60 
72 63 65 65 67 73 27 33 37 70 70 67 59 47 66 66 66 69 26 36 31 68 65 69 63 68 23 59 60 40 
33 34 68 62 69 72 70 59 65 76 76
EOF
#echo "Expected output:"
#cat $tempRef
diff $tempRef $tempOut

# Subset extraction
echo 'set unpack=1;set extractSubset=4;set doExtractSubsets=1;write;' | ${tools_dir}/codes_bufr_filter -o $tempBufr - $input
echo 'set unpack=1; assert(#2#percentConfidence==62); assert(#4#percentConfidence==63);' | ${tools_dir}/codes_bufr_filter - $tempBufr


# Unset environment variable and those percentConfidence keys should disappear
unset ECCODES_BUFR_QUALITY_WITHOUT_BITMAP
set +e
${tools_dir}/codes_bufr_filter $tempRules $input
status=$?
set -e
[ $status -ne 0 ]

rm -f $tempRules $tempRef $tempOut $tempBufr

#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh
set -u

label="bufr_ecc-1019_test_f"
TEMP_DUMP=temp.$label.dump
TEMP_TEXT=temp.$label.txt
TEMP_REF=temp.$label.ref
TEMP_BUFR=temp.$label.bufr

${examples_dir}/eccodes_f_bufr_ecc-1019 $TEMP_BUFR

${tools_dir}/codes_bufr_filter - $TEMP_BUFR > $TEMP_TEXT << EOF
  set unpack = 1;
  print " log3=[#3#log10OfPrincipalComponentsNormalizedFitToData]";
  print " ref=[log10OfPrincipalComponentsNormalizedFitToData->reference]";
  print " scR1=[#1#scaledIasiRadiance]";
  print " scR2=[#2#scaledIasiRadiance]";
EOF
cat $TEMP_TEXT

cat > $TEMP_REF << EOF
 log3=-1.1
 ref=-16383 -16383 -16383
 scR1=0
 scR2=0
EOF
diff $TEMP_REF $TEMP_TEXT

# Might as well check the JSON dump too
${tools_dir}/bufr_dump -ja $TEMP_BUFR > $TEMP_DUMP
grep -q '"reference" : -16383,' $TEMP_DUMP


# Clean up
rm -f $TEMP_BUFR $TEMP_DUMP $TEMP_TEXT $TEMP_REF

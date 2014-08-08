#!/bin/sh
# Copyright 2005-2014 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

cat > bufrdc_ref.filter<<EOF
set subsetNumber=0;
print "[numericValues!1%23.14e]";
EOF

for file in ${data_dir}/bufr/*.bufr
do
  REDIRECT=/dev/null

  # Test numeric data: compare output of filter (res) with reference file (ref)
  res_num=$file.num.test
  ref_num=$file.num.ref

  rm -f $res_num | true

  ${tools_dir}bufr_filter bufrdc_ref.filter $file 2> $REDIRECT > $res_num

  # Cannot use plain diff. We need to compare FLOAT NUMBERS with a tolerance
  #numdiff $ref_num $res_num >$REDIRECT
  perl number_compare.pl $ref_num $res_num >$REDIRECT 2> $REDIRECT

  rm -f $res_num

done

rm -f bufrdc_ref.filter

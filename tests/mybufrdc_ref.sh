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

REDIRECT=/dev/null

cat > bufrdc_num_ref.filter<<EOF
set subsetNumber=0;
print "[numericValues!1%23.14e]";
EOF

for file in ${data_dir}/bufr/*.bufr
do
  # Test numeric data: compare output of filter (res) with reference file (ref)
  res_num=$file.num.test
  ref_num=$file.num.ref
  diff_num=$file.num.diff

  rm -f $res_num

  set +e
  ${tools_dir}/codes_bufr_filter bufrdc_num_ref.filter $file 2> $REDIRECT > $res_num
  if [ $? != 0 ]
  then
    mv $file $file.no
  fi

  # Cannot use plain diff. We need to compare FLOAT NUMBERS with a tolerance
  perl number_compare.pl $ref_num $res_num > $diff_num 2> $diff_num
  #numdiff               $ref_num $res_num > $diff_num 2> $diff_num
  if [ $? != 0 ]
  then
    mv $file $file.no
  else 
    rm -f $res_num $diff_num
  fi

  #rm -f $res_num $diff_num

done

#rm -f bufrdc_num_ref.filter

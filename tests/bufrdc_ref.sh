#!/bin/sh
# Copyright 2005-2016 ECMWF.
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
print "[numericValues!1%23.14e]";
EOF

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`

for bf in ${bufr_files}
do
  file=${data_dir}/bufr/$bf
  # Test numeric data: compare output of filter (res) with reference file (ref)
  res_num=$file.num.test
  ref_num=$file.num.ref
  diff_num=$file.num.diff

  rm -f $res_num | true

  ${tools_dir}codes_bufr_filter bufrdc_num_ref.filter $file 2> $REDIRECT > $res_num

  # Exclude the BUFR file uegabe.bufr because its reference file is incorrect
  if [ "$bf" = "uegabe.bufr" ]; then
    continue
  fi

  if [ -f "$ref_num" ]; then
    # Cannot use plain diff. We need to compare FLOAT NUMBERS with a tolerance
    perl number_compare.pl $ref_num $res_num >$REDIRECT 2> $REDIRECT
    #numdiff               $ref_num $res_num >$REDIRECT 2> $REDIRECT
  fi

done

rm -f bufrdc_num_ref.filter

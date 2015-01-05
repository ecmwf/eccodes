#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

set -e
. ./include.sh

REDIRECT=/dev/null

cat > bufrdc_desc_ref.filter<<EOF
print "[bufrdcExpandedDescriptors!1''%06d]";
EOF

for file in ${data_dir}/bufr/*.bufr 
do
  REDIRECT=/dev/null

  # Test descriptors: compare output of filter (res) with reference file (ref)
  res_desc=$file.desc.test
  ref_desc=$file.desc.ref
  [ -f "$ref_desc" ]
  diff_desc=$file.desc.diff

  rm -f $res_desc

  ${tools_dir}bufr_filter bufrdc_desc_ref.filter $file 2> $REDIRECT > $res_desc

  diff $ref_desc $res_desc > $diff_desc 2> $diff_desc

  rm -f $res_desc $diff_desc
done

rm -f bufrdc_desc_ref.filter

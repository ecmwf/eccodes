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

  rm -f $file.test | true

  ${tools_dir}bufr_filter bufrdc_ref.filter $file 2> $REDIRECT > $file.test

  numdiff $file.ref $file.test

  rm -f $file.test


done

rm -f bufrdc_ref.filter

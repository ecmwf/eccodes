#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

label="get_product_kind_c"
fTmp=${label}.tmp
fOut=${label}.tmp.out
fRef=${label}.tmp.ref

# Create a file containing one GRIB and three BUFR messages
cat ${data_dir}/sample.grib2 ${data_dir}/bufr/syno_multi.bufr >$fTmp

${examples_dir}/c_get_product_kind $fTmp > $fOut

cat > $fRef <<EOF
message: 0
  product: GRIB
message: 1
  product: BUFR
message: 2
  product: BUFR
message: 3
  product: BUFR
EOF

diff ${fRef} ${fOut}

rm -f ${fTmp} ${fRef} ${fOut}

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

label="examples_f_get_product_kind"
fTmp=${label}.tmp
fOut=${label}.out

# Create a file containing both GRIB and BUFR messages
cat ${data_dir}/sample.grib2 ${data_dir}/bufr/syno_multi.bufr >$fTmp

${examples_dir}/f_get_product_kind $fTmp > $fOut
cat $fOut
grep -q "product: BUFR" $fOut
grep -q "product: GRIB" $fOut

rm -f $fTmp $fOut

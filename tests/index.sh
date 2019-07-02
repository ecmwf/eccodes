#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#


#set -x
. ./include.sh
infile=${data_dir}/index.grib

$EXEC ${test_dir}/index ${infile} > index.out

diff index.out ${data_dir}/index.ok

$EXEC ${test_dir}/read_index ${infile} > index.out

diff index.out ${data_dir}/index.ok

rm -f index.out out.gribidx

#-------------------------------
# Test grib_index_build
#-------------------------------
tempIndex=temp.$$.idx
tempOut=temp.$$.out
tempRef=temp.$$.ref
${tools_dir}/grib_index_build -o $tempIndex ${infile} >/dev/null

# Must remove first two lines and the last (filename specifics)
${tools_dir}/grib_dump -D ${tempIndex} | sed '1,2d' | sed '$d' > $tempOut
cat $tempOut

cat > $tempRef <<EOF
Index keys:
key name = mars.step
values = 12, 24, 48, 60
key name = mars.param
values = 130.128, 129.128
key name = mars.levelist
values = 10, 50, 100, 200, 250, 300, 400, 500, 700, 850, 925, 1000
key name = mars.number
values = 1, 2, 3, 4
Index count = 384
EOF

diff $tempRef $tempOut


${tools_dir}/grib_index_build -k mars.levtype -o $tempIndex ${data_dir}/tigge_cf_ecmwf.grib2 |\
   grep -q "mars.levtype = { sfc, pl, pv, pt }"

${tools_dir}/grib_index_build -k mars.levtype:i -o $tempIndex ${data_dir}/tigge_cf_ecmwf.grib2 |\
   grep -q "mars.levtype = { 103, 1, 106, 100, 101, 8, 109, 107 }"

rm -f $tempIndex $tempOut $tempRef

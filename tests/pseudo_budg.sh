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

label="pseudo_budg_test"

tempOut=temp.$label.txt
tempRef=temp.$label.ref
tempBud=temp.$label.bud

${tools_dir}/grib_ls -j ${data_dir}/budg > $tempOut
cat > $tempRef << EOF
{ "messages" : [ 
  {
    "identifier": "BUDG",
    "centre": "ecmf",
    "levelType": "sfc",
    "date": 20061204,
    "stepRange": 0,
    "parameter": 128
  }
]}
EOF
diff $tempRef $tempOut

# Set date
${tools_dir}/grib_set -s date=20170102 ${data_dir}/budg $tempBud
res=`${tools_dir}/grib_get -p mars.date $tempBud`
[ "$res" = "20170102" ]

${tools_dir}/grib_dump ${data_dir}/budg
${tools_dir}/grib_dump -O ${data_dir}/budg

ms=`${tools_dir}/grib_get -p mars.step ${data_dir}/budg`
[ "$ms" = "0" ]
${tools_dir}/grib_set -s mars.step=19 ${data_dir}/budg $tempOut
${tools_dir}/grib_ls -jm $tempOut
ms=`${tools_dir}/grib_get -p mars.step $tempOut`
[ "$ms" = "19" ]

# ECC-1491
cat ${data_dir}/budg ${data_dir}/budg > $tempBud
${tools_dir}/grib_get -p count,offset $tempBud > $tempOut
cat > $tempRef << EOF
1 0
2 6000
EOF
diff $tempRef $tempOut

# Count
count=`${tools_dir}/grib_count ${data_dir}/budg`
[ $count -eq 1 ]

rm -f $tempRef $tempOut $tempBud

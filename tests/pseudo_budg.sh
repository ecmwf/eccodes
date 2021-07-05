#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#


. ./include.sh

label="pseudo_budg_test"
set -u
tempOut=temp.$label.txt
tempRef=temp.$label.ref

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

${tools_dir}/grib_dump ${data_dir}/budg
${tools_dir}/grib_dump -O ${data_dir}/budg

rm -f $tempRef $tempOut

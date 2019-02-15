#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

tmpdata=grib_multi.$$.grib
rm -f $tmpdata

parameterNumber=`${tools_dir}/grib_get -p parameterNumber -w parameterCategory=2,parameterNumber=3 ${data_dir}/multi.grib2`
if [ -z "$parameterNumber"  ] 
then
  echo ---------- grib_get failure
  exit 1
fi

${tools_dir}/grib_copy -w parameterCategory=2,parameterNumber=3 ${data_dir}/multi.grib2  $tmpdata.1
${tools_dir}/grib_compare ${data_dir}/v.grib2 $tmpdata.1

cat > $tmpdata.rules <<EOF
if ( parameterCategory==2 && parameterNumber==3) {
  print "[parameterNumber]";
}
EOF

parameterNumber=`${tools_dir}/grib_filter $tmpdata.rules ${data_dir}/multi.grib2`
if [ -z "$parameterNumber"  ] 
then
  echo ---------- grib_filter failure
  exit 1
fi

rm -f $tmpdata.1 $tmpdata.rules

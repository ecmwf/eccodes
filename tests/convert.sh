#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

#
# ----------------------------------------------------------------------------------
# Note: the grib_convert tool is now deprecated! So this test is no longer relevant
# ----------------------------------------------------------------------------------
#

. ./include.sh

REDIRECT=/dev/null

cat > rules<<EOF
edition=2;
EOF

${tools_dir}grib_convert -g  rules ${data_dir}/gts.grib ${data_dir}/out.grib2

cgts=`${tools_dir}grib_count ${data_dir}/gts.grib`
cout=`${tools_dir}grib_count ${data_dir}/out.grib2`

if [ $cgts != $cout ]
then
  echo "error in gts test"
  exit 1
fi

rm -f ${data_dir}/out.grib2 rules


#if [ ! -f "${data_dir}/lfpw.grib1" ] 
#then
#  echo no data to convert
#  exit 0
#fi
#tmpdata=grib_api.grib
#rm -f $tmpdata || true
#${tools_dir}grib_convert ${data_dir}/mf.rules ${data_dir}/lfpw.grib1 $tmpdata 2> $REDIRECT > $REDIRECT
#${tools_dir}grib_compare -f ${data_dir}/lfpw.grib2 $tmpdata
#rm -f $tmpdata rules || true

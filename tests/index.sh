#!/bin/sh
# Copyright 2005-2018 ECMWF.
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

#if [ ! -f ${infile} ]
#then
#  echo no data to test
#  exit 0
#fi

${test_dir}/index ${infile} > index.out

diff index.out ${data_dir}/index.ok

${test_dir}/read_index ${infile} > index.out

diff index.out ${data_dir}/index.ok

rm -f index.out out.gribidx

#-------------------------------
# Test grib_index_build
#-------------------------------
tempIndex=temp.$$.ix
${tools_dir}/grib_index_build -o $tempIndex ${infile} >/dev/null
${tools_dir}/grib_dump -D ${tempIndex} > /dev/null

${tools_dir}/grib_index_build -k mars.levtype -o $tempIndex ${data_dir}/tigge_cf_ecmwf.grib2 |\
   grep -q "mars.levtype = { sfc, pl, pv, pt }"

${tools_dir}/grib_index_build -k mars.levtype:i -o $tempIndex ${data_dir}/tigge_cf_ecmwf.grib2 |\
   grep -q "mars.levtype = { 103, 1, 106, 100, 101, 8, 109, 107 }"

rm -f $tempIndex

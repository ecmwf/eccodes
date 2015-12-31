#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

infile=${data_dir}/mixed.grib
outfile=${data_dir}/read_any.out

if [ ! -f ${infile} ]
then
  echo no data to test
  exit 0
fi

$test_dir/read_any $infile > ${outfile}

diff ${outfile} ${data_dir}/read_any.ok

rm -f ${outfile}

count=`${tools_dir}/grib_count ${data_dir}/pad.grib`

if [ $count != 6 ]; then
  echo grib_io problem
  exit 1
fi

${tools_dir}/grib_count -v ${data_dir}/pad.grib >/dev/null

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

out1=${data_dir}/multi1.out
out2=${data_dir}/multi2.out

for infile in ${data_dir}/multi.grib2 ${data_dir}/reduced_gaussian_model_level.grib1
do
  if [ ! -f ${infile} ]
  then
    echo no data to test
    exit 0
  fi

  ${test_dir}/multi_from_message -m $infile > $out1
  ${tools_dir}grib_get -p count,shortName,level $infile > $out2
  diff $out1 $out2

  ${test_dir}/multi_from_message $infile > $out1
  ${tools_dir}grib_get -M -p count,shortName,level $infile > $out2
  diff $out1 $out2

  rm -f $out1 $out2 | true

done

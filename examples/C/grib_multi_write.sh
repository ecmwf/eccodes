#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

#if [ ! -f "${data_dir}/sample.grib2" ] 
#then
#  echo SKIP: $0
#  exit
#fi

${examples_dir}/c_grib_multi_write ${data_dir}/sample.grib2 ${data_dir}/multi_sample.grib2 > /dev/null

${tools_dir}/grib_get -p step ${data_dir}/multi_sample.grib2 > ${data_dir}/multi_step.test

diff ${data_dir}/multi_step.test ${data_dir}/multi_step.txt

step=`${tools_dir}/grib_get -M -p step ${data_dir}/multi_sample.grib2`

[ $step -eq 12 ]

rm -f ${data_dir}/multi_sample.grib2 ${data_dir}/multi_step.test

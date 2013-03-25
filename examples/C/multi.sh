#!/bin/sh
# Copyright 2005-2012 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

if [ ! -f "${data_dir}/multi.grib2" ] 
then
  echo SKIP: $0
  exit
fi
${examples_dir}multi > /dev/null



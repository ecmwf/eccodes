#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

numberOfValues=`${examples_dir}c_grib_print_data ../../data/constant_field.grib1 | grep values | awk '{print $1}'`

if [ $numberOfValues -ne 99200 ]
then
  echo ERROR: wrong number of values
  exit 1;
fi



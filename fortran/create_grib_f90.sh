#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
set -e
same=`./same_int_long`

if [ $same -eq 1 ]
then
 long=grib_f90_int.f90
 ec_long=eccodes_f90_int.f90
else
 long=grib_f90_long_int.f90
 ec_long=eccodes_f90_long_int.f90
fi

same=`./same_int_size_t`

if [ $same -eq 1 ]
then
 sizet=grib_f90_int_size_t.f90
 ec_sizet=eccodes_f90_int_size_t.f90
else
 sizet=grib_f90_long_size_t.f90
 ec_sizet=eccodes_f90_long_size_t.f90
fi

cat grib_f90_head.f90    $long    $sizet    grib_f90_tail.f90    > grib_f90.f90
cat eccodes_f90_head.f90 $ec_long $ec_sizet eccodes_f90_tail.f90 > eccodes_f90.f90

#!/bin/sh
# Copyright 2005-2012 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

same=`./same_int_long`

if [ $same -eq 1 ]
then
 interface=grib_f90_int.f90
else
 interface=grib_f90_long_int.f90
fi
cat grib_f90.f90.head $interface grib_f90.f90.tail > grib_f90.f90


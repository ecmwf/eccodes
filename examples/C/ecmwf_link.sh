#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

set -ex

CC=xlc
FC=xlf

jasper_lib="-I/usr/local/lib/metaps/lib/grib_api/jasper/include -L/usr/local/lib/metaps/lib/grib_api/jasper/lib -ljasper" 
grib_api_lib="-I/usr/local/lib/metaps/lib/grib_api/new/include -L/usr/local/lib/metaps/lib/grib_api/new/lib -lgrib_api"

$CC -o multi multi.c $jasper_lib $grib_api_lib -lm

./multi > /dev/null

$FC -o multi_fortran multi_fortran.F $jasper_lib $grib_api_lib

./multi_fortran > /dev/null



#!/bin/sh
set -ex

CC=xlc
FC=xlf

jasper_lib="-I/usr/local/lib/metaps/lib/grib_api/jasper/include -L/usr/local/lib/metaps/lib/grib_api/jasper/lib -ljasper" 
grib_api_lib="-I/usr/local/lib/metaps/lib/grib_api/new/include -L/usr/local/lib/metaps/lib/grib_api/new/lib -lgrib_api"

$CC -o multi multi.c $jasper_lib $grib_api_lib -lm

./multi > /dev/null

$FC -o multi_fortran multi_fortran.F $jasper_lib $grib_api_lib

./multi_fortran > /dev/null



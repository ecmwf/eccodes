#!/bin/sh
set -e

echo "-# grib_get fails if a key is not found.\\n "
echo "\\verbatim "
echo ">grib_get -p gribname ../data/tigge_pf_ecmwf.grib2"
set +e
./grib_get -p gribname ../data/tigge_pf_ecmwf.grib2
set -e
echo "\\endverbatim "
echo "-# To get the step of the first GRIB message in a file: \\n"
echo "\\verbatim "
echo ">grib_get -w count=1 -p step ../data/tigge_pf_ecmwf.grib2"
echo "\\endverbatim "



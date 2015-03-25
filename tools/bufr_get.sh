#!/bin/sh
set -e

echo "-# bufr_get fails if a key is not found.\\n "
echo "\\verbatim "
echo ">bufr_get -p centreName ../data/bufr/syno_1.bufr"
set +e
./grib_get -p gcentreName  ../data/bufr/syno_1.bufr
set -e
echo "\\endverbatim "

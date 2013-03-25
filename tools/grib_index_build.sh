#!/bin/sh
set -e

echo "-# By default grib_index_build will index on the MARS keys. \\n"
echo "\\verbatim "
echo "> grib_index_build ../data/reduced*.grib1 ../data/regular*.grib1 ../data/reduced*.grib2"
#grib_index_build ../data/reduced*.grib1 ../data/regular*.grib1 ../data/reduced*.grib2
echo "\\endverbatim\\n "

echo "-# To specify a custom list of keys to index on, use the -k option.\\n "
echo "\\verbatim "
echo "> grib_index_build -k paramId,dataDate ../data/reduced*.grib1 ../data/regular*.grib1 ../data/reduced*.grib2"
#grib_index_build -k paramId,dataDate ../data/reduced*.grib1 ../data/regular*.grib1 ../data/reduced*.grib2
echo "\\endverbatim\\n "

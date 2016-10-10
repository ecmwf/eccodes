#!/bin/sh

set -e

echo "-# To get a latitude, longitude, value list, skipping the missing values(=9999)"
echo "\\verbatim"
echo ">grib_get_data ../data/reduced_gaussian_model_level.grib2"
echo "\\endverbatim"

echo "-# If you want to define your missing value=1111 and to print the string 'missing' in place of it"
echo "\\verbatim"
echo ">grib_get_data -m 1111:missing ../data/reduced_gaussian_model_level.grib2"
echo "\\endverbatim"

echo "-# If you want to print the value of other keys with the data value list"
echo "\\verbatim"
echo ">grib_get_data -p centre,level,step ../data/reduced_gaussian_model_level.grib2"
echo "\\endverbatim"

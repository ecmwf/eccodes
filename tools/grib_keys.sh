#!/bin/sh
set -e

echo "-# With the -L option a list of the available templates is printed \\n"
echo "\\verbatim "
echo "> grib_keys -L \\n"
grib_keys -L
echo "\\endverbatim\\n "

echo "-# To print the standard set of key available for a given type\\n "
echo "\\verbatim "
echo "> grib_keys -T regular_ll_sfc_grib1"
#grib_keys -T regular_ll_sfc_grib1
echo "\\endverbatim\\n "


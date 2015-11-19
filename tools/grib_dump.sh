#!/bin/sh
set -e 

echo "-# To dump in a WMO documentation style with hexadecimal octet values (-H).\\n"
echo "\\verbatim"
echo  ">grib_dump -OH ../data/reduced_gaussian_model_level.grib1"
echo "\\endverbatim\\n"

echo "-# To add key aliases and type information.\\n"
echo "\\verbatim"
echo  ">grib_dump -OtaH ../data/reduced_gaussian_model_level.grib1"
echo "\\endverbatim\\n"

echo "-# To obtain all the key names (computed keys included) available in a grib file.\\n"
echo "\\verbatim "
echo "> grib_dump -D ../data/regular_latlon_surface.grib1"
echo "\\endverbatim\\n"

#echo "-# To obtain a C code example from a grib file.\\n"
#echo "\\code "
#echo ">grib_dump -C ../data/regular_latlon_surface.grib1"
#./grib_dump -C ../data/regular_latlon_surface.grib1
#echo "\\endcode\\n"


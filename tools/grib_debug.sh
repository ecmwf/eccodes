#!/bin/sh
set -e 

echo "Dumping in a WMO documentation style with hexadecimal octet values (-H)\\n"
echo "and with the aliases of each key listed in square brackets (-a).\\n\\n"
echo  grib_dump -Ha ../data/reduced_gaussian_model_level.grib1\\n
echo "\\verbatim"
./grib_dump -Ha ../data/reduced_gaussian_model_level.grib1
echo "\\endverbatim\\n"

echo "\\n \\n How to obtain all the key names available in a grib file.\\n\\n"
echo grib_dump -D ../data/regular_latlon_surface.grib1
echo "\\verbatim "
./grib_dump -D ../data/regular_latlon_surface.grib1
echo "\\endverbatim\\n"

echo "\\n\\n How to obtain a C code example from a grib file.\\n\\n"
echo grib_dump -C ../data/regular_latlon_surface.grib1\\n
echo "\\code "
./grib_dump -C ../data/regular_latlon_surface.grib1
echo "\\endcode\\n"


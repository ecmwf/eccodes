#!/bin/sh
set -e

echo "-# Without options a default list of keys is printed. \\n"
echo "The default list is different depending on the type of grib message. \\n"
echo "\\verbatim "
echo "> grib_ls ../data/reduced*.grib1 ../data/regular*.grib1 ../data/reduced*.grib2 \\n"
#grib_ls -W 8 ../data/reduced*.grib1 ../data/regular*.grib1 ../data/reduced*.grib2
echo "\\endverbatim\\n "

echo "-# To print offset and count number in file use the keys offset and count\\n "
echo "Also the total count in a set of files is available as countTotal\\n"
echo "\\verbatim "
echo "> grib_ls -p offset,count,countTotal ../data/reduced*.grib1"
#grib_ls -p offset,count,countTotal ../data/reduced*.grib1
echo "\\endverbatim\\n "

echo "-# To list only a subset of messages use the -w (where option).\\n "
echo "Only the pressure levels are listed with the following line.\\n "
echo "\\verbatim "
echo "> grib_ls -w levelType=pl ../tigge_pf_ecmwf.grib2 "
#grib_ls -W 8 -w levelType=pl ../data/tigge_pf_ecmwf.grib2
echo "\\endverbatim\\n "

echo "-# All the grib messages not on pressure levels are listed as follows:\\n "
echo "\\verbatim "
echo "> grib_ls -w levelType!=pl ../tigge_pf_ecmwf.grib2 "
#grib_ls -W 8 -w levelType!=pl ../data/tigge_pf_ecmwf.grib2
echo "\\endverbatim\\n "

echo "-# To get the closest grid point to a latitude/longitude.\\n"
echo "\\code "
echo ">grib_ls -l 51.46,-1.33,1 -p paramId,name ../data/reduced_gaussian_surface.grib2"
./grib_ls -l 51.46,-1.33,1 -p paramId,shortName ../data/reduced_gaussian_surface.grib2
echo "\\endcode\\n"

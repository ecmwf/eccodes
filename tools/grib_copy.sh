#!/bin/sh
set -e

echo "-# To copy only the pressure levels from a file\\n"
echo "\verbatim"
echo "> grib_copy -w levtype=pl ../data/tigge_pf_ecmwf.grib2 out.grib"
echo "\endverbatim\\n"

echo "-# To copy only the fields that are not on pressure levels from a file\\n"
echo "\verbatim"
echo "> grib_copy -w levtype!=pl ../data/tigge_pf_ecmwf.grib2 out.grib"
echo "\endverbatim\\n"

echo "-# To copy only the first three fields from a file\\n"
echo "\verbatim"
echo "> grib_copy -w count=1/2/3 ../data/tigge_pf_ecmwf.grib2 out.grib"
echo "\endverbatim\\n"

echo "-# A grib_file with multi field messages can be converted in single field messages"
echo "with a simple grib_copy.\\n"
echo "\verbatim"
echo "> grib_copy multi.grib simple.grib"
echo "\endverbatim\\n"

echo "-# Use the square brackets to insert the value of a key"
echo "in the name of the output file (This is a good way to split a large GRIB file)\\n"
echo "\verbatim"
echo "> grib_copy in.grib 'out_[shortName].grib'"
echo "\endverbatim\\n"
echo "Note: we need to quote the name of the output so the shell does not interpret the square brackets"

echo "-# To copy fields whose typeOfLevel is either 'surface' or 'meanSea'"
echo "\verbatim"
echo "> grib_copy -w typeOfLevel=surface/meanSea orig.grib out.grib"
echo "\endverbatim\\n"

echo "-# To copy selected fields and apply sorting (sorted by level in ascending order)"
echo "\verbatim"
echo "> grib_copy -w typeOfLevel=heightAboveGround -B'level:i asc' tigge_af_ecmwf.grib2 out.grib"
echo "\endverbatim\\n"
echo "Note: we need to specify the ':i' to get a numerical sort. By default values are sorted as strings so a level of 100 would come before 20!"

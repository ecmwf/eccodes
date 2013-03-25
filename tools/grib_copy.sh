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

echo "-# A grib_file with multi field messages can be converted in single field messages"
echo "with a simple grib_copy.\\n"
echo "\verbatim"
echo "> grib_copy multi.grib simple.grib"
echo "\endverbatim\\n"

echo "-# Use the square brackets to insert the value of a key"
echo "in the name of the output file.\\n"
echo "\verbatim"
echo "> grib_copy in.grib out_[shortName].grib"
echo "\endverbatim\\n"

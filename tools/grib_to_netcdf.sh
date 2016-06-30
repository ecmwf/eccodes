#!/bin/sh

set -e

echo "-# Produce a NetCDF file from grib edition 1"
echo "\\verbatim"
echo ">grib_to_netcdf -o output.nc input.grib1"
echo "\\endverbatim"

echo "-# If your grib file has analysis and 6-hour forecast, then ignore keys 'type' and 'step'. Thus type=an/fc and step=00/06 will not be considered as netcdf dimensions."
echo "\\verbatim"
echo ">grib_to_netcdf -I type,step -o output.nc input.grib"
echo "\\endverbatim"

echo "-# Do not use time of validity. If time of validity is used, it means the 1D time coordinate is considered as date+time+step, otherwise 3 different dimensions are created."
echo "The default behaviour is to use the time of validity."
echo "\\verbatim"
echo ">grib_to_netcdf -T -o output.nc input.grib"
echo "\\endverbatim"

echo "-# Produce NetCDF with data type of FLOAT (32bit floating point, for higher precision)."
echo "Note these types were chosen to provide a reasonably wide range of trade-offs between data precision and number of bits required for each value"
echo "\\verbatim"
echo ">grib_to_netcdf -D NC_FLOAT -o output.nc input.grib"
echo "\\endverbatim"

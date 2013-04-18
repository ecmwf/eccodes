#!/bin/sh
# Copyright 2005-2013 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh
#set -x

# First decide if we have NetCDF enabled from configure.
# Temporarily turn off the '-e' flag for set so we dont exit on error
set +e
tmp_out=tmp_msg.out
skip_test=0
rm -f $tmp_out | true
# Invoke the grib_to_netcdf command with no options. If NetCDF was enabled
# we get a usage message otherwise we get a specific message. Note: In both cases
# the command fails.
${tools_dir}grib_to_netcdf > $tmp_out
grep 'GRIB API was not compiled with NETCDF enabled' $tmp_out > /dev/null
if [ $? -eq 0 ]; then
  # Message was found
  skip_test=1
fi
# Restore the set -e so any error causes us to exit
set -e
rm -f $tmp_out | true
if [ $skip_test -eq 1 ]; then
  echo "NetCDF was not enabled. Skipping this test."
  exit 0
fi
echo "NetCDF was enabled."

grib_files="tigge_pf_ecmwf.grib2 \
set.grib2 set.grib1 \
regular_latlon_surface.grib1.1 \
regular_latlon_surface.grib2 \
regular_latlon_surface.grib1 \
regular_gaussian_model_level.grib1 \
regular_gaussian_model_level.grib2 \
regular_gaussian_pressure_level.grib1 \
regular_gaussian_pressure_level.grib2 \
regular_gaussian_surface.grib1 \
regular_gaussian_surface.grib2 \
lfpw.grib2 \
missing.grib2"

ncf_types="NC_BYTE NC_SHORT NC_INT NC_FLOAT NC_DOUBLE"

tmp_netcdf=tmp.ncf

# Go thru all the specified GRIB files and convert them to NetCDF
for dt in $ncf_types
do
   for f in $grib_files
   do
      rm -f $tmp_netcdf | true
      #echo Doing $f with $dt...
      if [ -f "${data_dir}/$f" ]; then
        ${tools_dir}grib_to_netcdf -D $dt -o $tmp_netcdf ${data_dir}/$f >/dev/null
      fi
   done
done
rm -f $tmp_netcdf | true


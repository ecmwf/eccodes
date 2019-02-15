#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh
#set -x

# Disable if autotools being used
src_config=${src_dir}/config.h
if [ -f ${src_config} ]; then
  exit 0
fi

grib_files="\
 regular_latlon_surface.grib2 \
 regular_latlon_surface.grib1 \
 regular_gaussian_model_level.grib1 \
 regular_gaussian_pressure_level.grib2 \
 regular_gaussian_surface.grib1 \
 missing.grib2"

ncf_types="NC_SHORT NC_INT NC_FLOAT NC_DOUBLE"

tmp_netcdf=tmp.ncf

# Go thru all the specified GRIB files and convert them to NetCDF
for dt in $ncf_types
do
   for f in $grib_files
   do
      rm -f $tmp_netcdf
      [ -f ${data_dir}/$f ]
      ${tools_dir}/grib_to_netcdf -D $dt -o $tmp_netcdf ${data_dir}/$f >/dev/null
      ${tools_dir}/grib_to_netcdf -T -o $tmp_netcdf ${data_dir}/$f >/dev/null
   done
done

# Try creating different kinds; netcdf3 classic and large
# TODO: enable tests for netcdf4 formats too
input=${data_dir}/regular_latlon_surface.grib2
${tools_dir}/grib_to_netcdf -k 1 -o $tmp_netcdf $input >/dev/null
${tools_dir}/grib_to_netcdf -k 2 -o $tmp_netcdf $input >/dev/null
#${tools_dir}/grib_to_netcdf -k 3 -o $tmp_netcdf $input >/dev/null
#${tools_dir}/grib_to_netcdf -k 4 -o $tmp_netcdf $input >/dev/null

rm -f $tmp_netcdf

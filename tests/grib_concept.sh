#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

# data="${data_dir}/*grib1 ${data_dir}/*grib2"
data="
constant_field.grib1
lfpw.grib1
missing_field.grib1
reduced_gaussian_lsm.grib1
reduced_gaussian_model_level.grib1
reduced_gaussian_pressure_level.grib1
reduced_gaussian_pressure_level_constant.grib1
reduced_gaussian_sub_area.grib1
reduced_gaussian_surface.grib1
reduced_latlon_surface.grib1
reduced_latlon_surface_constant.grib1
reference_ensemble_mean.grib1
reference_stdev.grib1
regular_gaussian_model_level.grib1
regular_gaussian_pressure_level.grib1
regular_gaussian_pressure_level_constant.grib1
regular_gaussian_surface.grib1
regular_latlon_surface.grib1
regular_latlon_surface_constant.grib1
second_ord_rbr.grib1
small_ensemble.grib1
spectral_complex.grib1
spherical_model_level.grib1
spherical_pressure_level.grib1
test.grib1

constant_field.grib2
gfs.c255.grib2
jpeg.grib2
missing.grib2
multi.grib2
multi_created.grib2
reduced_gaussian_model_level.grib2
reduced_gaussian_pressure_level.grib2
reduced_gaussian_pressure_level_constant.grib2
reduced_gaussian_sub_area.grib2
reduced_gaussian_surface.grib2
reduced_gaussian_surface_jpeg.grib2
reduced_latlon_surface.grib2
reduced_latlon_surface_constant.grib2
regular_gaussian_model_level.grib2
regular_gaussian_pressure_level.grib2
regular_gaussian_pressure_level_constant.grib2
regular_gaussian_surface.grib2
regular_latlon_surface.grib2
regular_latlon_surface_constant.grib2
sample.grib2
spherical_model_level.grib2
spherical_pressure_level.grib2
test_uuid.grib2
tigge_af_ecmwf.grib2
tigge_cf_ecmwf.grib2
tigge_ecmwf.grib2
tigge_pf_ecmwf.grib2
v.grib2
"

tmp="tmp.concept.out"

for d in $data
do
  set -e
  ${tools_dir}/grib_ls -p gridType,packingType -w dataRepresentationType!=192 ${data_dir}/$d > $tmp
 
  set +e
  grep unknown $tmp
 
  if [ $? -eq 0 ]; then 
    false
    exit
  fi
  rm -f $tmp

done

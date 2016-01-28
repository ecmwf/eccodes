#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

# Check Gaussian grids
files_global="
    reduced_gaussian_lsm.grib1
    reduced_gaussian_model_level.grib1
    reduced_gaussian_pressure_level.grib1
    reduced_gaussian_pressure_level_constant.grib1
    reduced_gaussian_surface.grib1
    reference_ensemble_mean.grib1
    reference_stdev.grib1
    regular_gaussian_model_level.grib1
    regular_gaussian_pressure_level.grib1
    regular_gaussian_pressure_level_constant.grib1
    regular_gaussian_surface.grib1
    reduced_gaussian_model_level.grib2
    reduced_gaussian_pressure_level.grib2
    reduced_gaussian_pressure_level_constant.grib2
    reduced_gaussian_surface.grib2
    reduced_gaussian_surface_jpeg.grib2
    regular_gaussian_model_level.grib2
    regular_gaussian_pressure_level.grib2
    regular_gaussian_pressure_level_constant.grib2
    regular_gaussian_surface.grib2
    tigge/tigge_ecmf_pl_gh.grib
    tigge/tigge_sbsj_pl_q.grib
"

files_subarea="
    reduced_gaussian_sub_area.grib1
    reduced_gaussian_sub_area.grib2
"

for gg in $files_global; do
    filepath=${data_dir}/$gg
    grib_check_key_equals $filepath "global" 1
done

for gg in $files_subarea; do
    filepath=${data_dir}/$gg
    grib_check_key_equals $filepath "global" 0
done

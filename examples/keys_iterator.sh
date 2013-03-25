#!/bin/sh

. ./include.sh

${examples_dir}keys_iterator ${data_dir}/reduced_latlon_surface.grib1 > /dev/null

${examples_dir}keys_iterator ${data_dir}/regular_gaussian_model_level.grib1 > /dev/null


#!/bin/sh

. ./include.sh

${examples_dir}iterator ${data_dir}/reduced_gaussian_model_level.grib1 > /dev/null

${examples_dir}iterator ${data_dir}/regular_gaussian_model_level.grib1 > /dev/null


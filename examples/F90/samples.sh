#!/bin/sh
# Copyright 2005-2013 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

# Take a GRIB file in the data folder and make it a sample file (extension .tmpl)
rm -f ${data_dir}/regular_latlon_surface.grib1.tmpl | true
cp ${data_dir}/regular_latlon_surface.grib1 ${data_dir}/regular_latlon_surface.grib1.tmpl

# Change the GRIB_SAMPLES_PATH: put the data dir first
GRIB_SAMPLES_PATH=${data_dir}:${samples_dir}
export GRIB_SAMPLES_PATH
${examples_dir}samples > /dev/null

# Extend the GRIB_SAMPLES_PATH: put the data dir second
GRIB_SAMPLES_PATH=${samples_dir}:${data_dir}
export GRIB_SAMPLES_PATH
${examples_dir}samples > /dev/null


#!/bin/sh
# Copyright 2005-2014 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

${examples_dir}set > /dev/null

# Create the grib2 file needed for this test
grib1File=${data_dir}/reduced_gaussian_pressure_level.grib1
grib2File=${data_dir}/reduced_gaussian_pressure_level.grib2
${tools_dir}grib_set -s editionNumber=2 $grib1File $grib2File

${examples_dir}set_missing > /dev/null

rm -f out.grib1 out_surface_level.grib2 $grib2File

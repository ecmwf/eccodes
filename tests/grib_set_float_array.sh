#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

# codes_set_float_array must give the message of codes_set_double_array with d[i]=(double)v[i],
# for every packing: simple packing with and without the new implementation (ECCODES_USE_SP) and
# CCSDS with and without SpAec (ECCODES_USE_SPAEC). The environment variables are read once per
# process: one program run per mode.
# GRIB2 grid_simple (test_file.grib2), grid_ccsds (ccsds_szip.grib2), both with a bitmap
# (reduced_latlon_surface.grib2), a constant field (constant_field.grib2), GRIB1 grid_simple
# with and without a bitmap (regular_latlon_surface.grib1, sst_globus0083.grib) and a sample

label="grib_set_float_array_test"
tempCcsdsBitmap=temp.$label.ccsds_bitmap.grib2
${tools_dir}/grib_set -r -s packingType=grid_ccsds ${data_dir}/reduced_latlon_surface.grib2 $tempCcsdsBitmap

files="${data_dir}/test_file.grib2 ${data_dir}/ccsds_szip.grib2 ${data_dir}/reduced_latlon_surface.grib2 $tempCcsdsBitmap
       ${data_dir}/constant_field.grib2 ${data_dir}/regular_latlon_surface.grib1 ${data_dir}/sst_globus0083.grib ${samp_dir}/GRIB2.tmpl"

for spaec in 0 1; do
    for sp in 0 1; do
        ECCODES_USE_SPAEC=$spaec ECCODES_USE_SP=$sp ${test_dir}/grib_set_float_array $files
    done
done

rm -f $tempCcsdsBitmap

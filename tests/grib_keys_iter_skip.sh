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

label="grib_keys_iter_skip_test"

gribs="
    row.grib
    grid_ieee.grib
    reduced_gaussian_model_level.grib1
    sample.grib2
    gfs.c255.grib2
"
for f in $gribs; do
    g=$data_dir/$f
    ${test_dir}/grib_keys_iter_skip $g
done

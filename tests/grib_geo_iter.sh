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

label="grib_geo_iter_test"

infiles="
    $ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
    $ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib1.tmpl
    $ECCODES_SAMPLES_PATH/regular_gg_ml_grib2.tmpl
    $ECCODES_SAMPLES_PATH/polar_stereographic_pl_grib2.tmpl
    $data_dir/regular_latlon_surface.grib1
    $data_dir/mercator.grib2
"

# Run the iterator in two modes:
#  -v  decodes the values
#  -n  does not decode the values
infile=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
for infile in $infiles; do
    $EXEC ${test_dir}/grib_geo_iter -v $infile
    $EXEC ${test_dir}/grib_geo_iter -n $infile
done

# Test a case where decoding is not possible but the iterator would work
if [ $HAVE_JPEG -eq 0 ]; then
    # No JPEG, so cannot decode the field but the iterator doesn't need to
    infile=$data_dir/jpeg.grib2
    set +e
    ${tools_dir}/grib_get -p min,max $infile
    status=$?
    set -e
    [ $status -ne 0 ]  # Make sure it fails to decode

    $EXEC ${test_dir}/grib_geo_iter -n $infile
fi

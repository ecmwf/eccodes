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

label="grib_lam_bf"
temp=temp.$label.txt

dump_and_check()
{
    input=$1
    numberOfValues=$2

    grib_check_key_equals $input numberOfValues $numberOfValues

    ${tools_dir}/grib_dump -O $input >$temp 2>&1
    set +e
    # Look for the word ERROR in output. We should not find any
    grep -q 'ERROR ' $temp
    if [ $? -eq 0 ]; then
        echo "File $input: found string ERROR in grib_dump output!"
        exit 1
    fi
    set -e
}

# Spectral LAM fields (Bi-Fourier)
# Run the test which generates the GRIB files
${test_dir}/grib_lam_bf

dump_and_check lam_bf_lambert_bf_ellipse_diamond.grib                  3128
dump_and_check lam_bf_mercator_bf_ellipse_diamond.grib                 3128
dump_and_check lam_bf_polar_stereographic_bf_ellipse_diamond.grib      3128

dump_and_check lam_bf_lambert_bf_rectangle_rectangle.grib              4096
dump_and_check lam_bf_mercator_bf_rectangle_rectangle.grib             4096
dump_and_check lam_bf_polar_stereographic_bf_rectangle_rectangle.grib  4096

# Clean up test outputs
rm -f $temp
rm -f lam_bf_lambert_bf_ellipse_diamond.grib
rm -f lam_bf_mercator_bf_ellipse_diamond.grib
rm -f lam_bf_polar_stereographic_bf_ellipse_diamond.grib
rm -f lam_bf_lambert_bf_rectangle_rectangle.grib
rm -f lam_bf_mercator_bf_rectangle_rectangle.grib
rm -f lam_bf_polar_stereographic_bf_rectangle_rectangle.grib

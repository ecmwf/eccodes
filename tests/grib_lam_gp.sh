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

label="grib_lam_gp_test"
temp=temp.$label.txt

dump_and_check()
{
    input=$1

    grib_check_key_equals $input numberOfValues 4096

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

# Grid-point LAM fields
# Run the test which generates the GRIB files
$EXEC ${test_dir}/grib_lam_gp

dump_and_check lam_gp_lambert_lam.grib
dump_and_check lam_gp_mercator_lam.grib
dump_and_check lam_gp_polar_stereographic_lam.grib

# Clean up test outputs
rm -f $temp
rm -f lam_gp_lambert_lam.grib
rm -f lam_gp_mercator_lam.grib
rm -f lam_gp_polar_stereographic_lam.grib

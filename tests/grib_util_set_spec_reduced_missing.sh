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

# Test that iDirectionIncrement and Ni are set to MISSING for reduced grids
# See commit bc0ae2be9342: "eccodes + metkit: iDirectionIncrement/Ni=MISSING for reduced grids"


if [ $HAVE_ECKIT_GEO -ne 1 ]; then
    echo "$0: This test is disabled when HAVE_ECKIT_GEO=OFF"
    exit 0
fi

# Check env. variable too
if [ "${ECCODES_ECKIT_GEO:-0}" -eq 0 ]
then
    echo "$0: This test is disabled (env. variable ECCODES_ECKIT_GEO=0)"
    exit 0
fi

$EXEC ${test_dir}/grib_util_set_spec_reduced_missing

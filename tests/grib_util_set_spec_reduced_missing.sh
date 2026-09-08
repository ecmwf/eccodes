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

$EXEC ${test_dir}/grib_util_set_spec_reduced_missing

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

unset ECCODES_SAMPLES_PATH
IFS_SAMPLES_ROOT=${proj_dir}/ifs_samples

# Load the given sample (arg1) which resides in the ifs_samples dir (arg2)
$EXEC ${test_dir}/codes_set_samples_path "gg_ml" "$IFS_SAMPLES_ROOT/grib1_mlgrib2"

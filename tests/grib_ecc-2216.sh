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

# ---------------------------------------------------------
# This is the test for JIRA issue ECC-2216
# The purpose of this test is to detect data races during accessor initialization.
# It only works when used with Valgrind or with the -fsanitize=thread compiler option.
# ---------------------------------------------------------

export TSAN_OPTIONS='ignore_noninstrumented_modules=1'

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
$EXEC ${test_dir}/grib_ecc-2216 $sample_grib2 8

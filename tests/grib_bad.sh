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

REDIRECT=/dev/null

# Do not fail on wrong length
${tools_dir}/grib_ls -7 ${data_dir}/bad.grib 2> $REDIRECT > $REDIRECT

# Do fail on wrong length
set +e
${tools_dir}/grib_ls ${data_dir}/bad.grib 2> $REDIRECT > $REDIRECT
status=$?
set -e
[ $status -ne 0 ]

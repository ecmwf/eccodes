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

label="big2gribex_test"
tempGrib=temp.$label.grib

if [ ! -x ${tools_dir}/big2gribex ]; then
    exit 0
fi

set +e
${tools_dir}/big2gribex
status=$?
set -e
[ $status -ne 0 ]

${tools_dir}/big2gribex $data_dir/test.grib1 $tempGrib

# Clean up
rm -f $tempGrib

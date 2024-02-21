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

label="codes_codetable_test"
tempGrib=temp.$label.grib
tempLog=temp.$label.log

$EXEC ${test_dir}/codes_codetable

# Clean up
rm -f $tempGrib $tempLog

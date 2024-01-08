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

label='grib_set_large_message_fail_test'
temp=temp.$label.txt

$EXEC ${test_dir}/grib_set_large_message_fail > $temp 2>&1
grep -q "Failed to set GRIB1 message length" $temp

$EXEC ${test_dir}/grib_set_large_message_fail -b > $temp 2>&1
grep -q "Unable to set double array.*bitmap" $temp

# Clean up
rm -f $temp

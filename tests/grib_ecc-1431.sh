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
set -u
REDIRECT=/dev/null
label="grib_ECC-1431_test"  # Change prod to bufr or grib etc
temp=temp.$label

$EXEC $test_dir/grib_ecc-1431

rm -f $temp

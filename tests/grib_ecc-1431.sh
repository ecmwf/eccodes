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

# ---------------------------------------------------------------------
# This is the test for JIRA issue ECC-1431
# CCSDS encoding failure AEC_STREAM_ERROR
# ---------------------------------------------------------------------

label="grib_ecc-1431_test"
temp=temp.$label

$EXEC $test_dir/grib_ecc-1431

rm -f $temp

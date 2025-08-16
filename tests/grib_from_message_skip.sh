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

label="grib_from_message_skip_test"
temp=temp.$label.txt
tempGrib=temp.$label.grib

# Test with GRIBs which have no padding. First 4 bytes are 'GRIB'
$EXEC ${test_dir}/grib_from_message_skip ${data_dir}/sample.grib2
$EXEC ${test_dir}/grib_from_message_skip $ECCODES_SAMPLES_PATH/GRIB1.tmpl
$EXEC ${test_dir}/grib_from_message_skip $ECCODES_SAMPLES_PATH/GRIB2.tmpl


# Test with GRIBs which have padding. The 'GRIB' marker is at byte 41
$EXEC ${test_dir}/grib_from_message_skip -p ${data_dir}/gts.grib > $temp 2>&1
grep -q "No final 7777 in message" $temp

$EXEC ${test_dir}/grib_from_message_skip -p ${data_dir}/pad.grib > $temp 2>&1
grep -q "No final 7777 in message" $temp

# BUFR padding followed by GRIB
cat $ECCODES_SAMPLES_PATH/BUFR4.tmpl $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempGrib
$EXEC ${test_dir}/grib_from_message_skip -p $tempGrib


# Error conditions
set +e
$EXEC ${test_dir}/grib_from_message_skip $ECCODES_SAMPLES_PATH/BUFR4.tmpl > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Message is not GRIB" $temp


# Clean up
rm -f $temp $tempGrib

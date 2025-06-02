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

label="wmo_read_any_from_file_test"
tempText=temp.$label.txt
tempBufr=temp.$label.bufr

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

${test_dir}/wmo_read_any_from_file $data_dir/bufr/aaen_55.bufr
${test_dir}/wmo_read_any_from_file $data_dir/sample.grib2
${test_dir}/wmo_read_any_from_file $data_dir/second_ord_rbr.grib1
${test_dir}/wmo_read_any_from_file $data_dir/gts/EGRR20150317121020_00493212.DAT
${test_dir}/wmo_read_any_from_file $ECCODES_SAMPLES_PATH/wrap.tmpl
${test_dir}/wmo_read_any_from_file $ECCODES_SAMPLES_PATH/budg.tmpl
${test_dir}/wmo_read_any_from_file $ECCODES_SAMPLES_PATH/hdf5.tmpl

# Bad input
echo BUFR > $tempBufr
set +e
${test_dir}/wmo_read_any_from_file $tempBufr > $tempText
status=$?
set -e
[ $status -ne 0 ]
cat $tempText
grep -q "End of resource reached when reading message" $tempText


set +e
${test_dir}/wmo_read_any_from_file $data_dir > $tempText
status=$?
set -e
[ $status -ne 0 ]
cat $tempText
grep -q "Input output problem" $tempText


# Clean up
rm -f $tempText $tempBufr

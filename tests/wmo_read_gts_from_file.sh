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

label="wmo_read_gts_from_file_test"
tempText=temp.$label.txt

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

input=${data_dir}/gts/EGRR20150317121020_00493212.DAT
${test_dir}/wmo_read_gts_from_file $input > $tempText
cat $tempText


# Clean up
rm -f $tempText

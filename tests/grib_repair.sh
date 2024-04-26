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

label="grib_repair_test"
tempText=temp.$label.txt
tempGoodGribs=temp.$label.good.grib
tempBadGribs=temp.$label.bad.grib

if [ ! -e "${tools_dir}/grib_repair" ]; then
    exit 0
fi

set +e
${tools_dir}/grib_repair
status=$?
set -e
[ $status -ne 0 ]


export ECCODES_GRIB_REPAIR_MAX_NUM_MESSAGES=3
${tools_dir}/grib_repair $data_dir/bad.grib $tempGoodGribs $tempBadGribs > $tempText 2>&1
grep -q "Wrong message length" $tempText

count=$( ${tools_dir}/grib_count $tempGoodGribs )
[ $count -eq 1 ]

count=$( ${tools_dir}/grib_count $tempBadGribs )
[ $count -eq 3 ]

${tools_dir}/grib_ls $tempGoodGribs
${tools_dir}/grib_ls $tempBadGribs


# Clean up
rm -f $tempText $tempGoodGribs $tempBadGribs

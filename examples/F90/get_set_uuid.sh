#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh


uuid=`${tools_dir}/grib_get -w count=1 -p uuidOfVGrid:s ${data_dir}/test_uuid.grib2`
[ "$uuid" = "08b1e836bc6911e1951fb51b5624ad8d" ]

# This reads the file in data/test_uuid.grib2 and creates test_uuid.grib2
${examples_dir}eccodes_f_get_set_uuid > /dev/null

# Check output was written
output=out_uuid.grib2

[ -f "$output" ]

uuid=`${tools_dir}/grib_get -w count=1 -p uuidOfVGrid:s $output`
[ "$uuid" = "8dad24561bb51f95e11169bc36e8b108" ]

rm -f $output

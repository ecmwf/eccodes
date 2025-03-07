#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label="examples_c_grib_index"
tempIndex=temp.$label.idx
tempText=temp.$label.txt

input=$data_dir/tigge_cf_ecmwf.grib2
${examples_dir}/c_grib_index $input $tempIndex > $tempText

grep -q "43 messages selected" $tempText

${tools_dir}/grib_dump $tempIndex

rm -f $tempIndex $tempText

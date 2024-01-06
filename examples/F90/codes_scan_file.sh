#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label='eccodes_f_codes_scan_file'
temp=temp.$label.txt

# The input file is hard coded => data/index.grib
${examples_dir}/eccodes_f_codes_scan_file > $temp

grep -q "Num messages= *384" $temp
grep -q "level= *700  step= *60" $temp

rm -f $temp

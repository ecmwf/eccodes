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

label="codes_dump_content_test"
temp=temp.$label.txt

infile=$data_dir/sample.grib2

modes="default wmo json serialize debug grib_encode_C"
for mode in $modes; do
    $EXEC ${test_dir}/codes_dump_content $mode $infile
done

${test_dir}/codes_dump_content rubbish $infile > $temp 2>&1
grep -q "ERROR.*Unknown type : 'rubbish' for dumper" $temp

# Clean up
rm -f $temp

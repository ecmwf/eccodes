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

# Single-precision access (codes_get_float_array, codes_set_float_array) to BUFR numeric keys.
# Run over all BUFR test files (compressed and uncompressed).

label="bufr_float_test"
tempLog=temp.$label.log

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
for f in $bufr_files; do
    $EXEC ${test_dir}/bufr_float ${data_dir}/bufr/$f > $tempLog
done

rm -f $tempLog

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

label="bufr_keys_iter_header_test"
tempOut=temp.$label.out
tempRef=temp.$label.ref

input=$ECCODES_SAMPLES_PATH/BUFR4_local.tmpl
$EXEC ${test_dir}/bufr_keys_iter_header $input

input=$ECCODES_SAMPLES_PATH/BUFR4_local_satellite.tmpl
$EXEC ${test_dir}/bufr_keys_iter_header $input

rm -f $tempRef $tempOut

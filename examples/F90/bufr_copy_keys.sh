#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

#Define a common label for all the tmp files
label="bufr_copy_keys_test_f"

TEMP=$label.out.bufr

cd ${data_dir}/bufr

#The input and output BUFR files are hardcoded in the f90 example!!!
OUTPUT=out.$label.bufr
${examples_dir}/eccodes_f_bufr_copy_keys
res=`${tools_dir}/bufr_get -p bufrHeaderCentre $OUTPUT`
[ "$res" = "80" ]

rm -f $OUTPUT

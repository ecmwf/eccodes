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

# Define a common label for all the tmp files
label="examples_f_bufr_keys_iterator"

fTmp=${label}".tmp.txt"
rm -f $fTmp

f=${data_dir}/bufr/syno_1.bufr

# The input ($f) is hard coded in the example
${examples_dir}/f_bufr_keys_iterator > $fTmp

# Check the output
grep -q '#49#dataPresentIndicator' $fTmp
grep -q '#1#generatingApplication' $fTmp

rm -f $fTmp

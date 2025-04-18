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
label="examples_f_bufr_read_synop_test"

fTmp=${label}".tmp.txt"
rm -f $fTmp

# We check "syno_multi.bufr". The path is hard coded in the example
${examples_dir}/f_bufr_read_synop > $fTmp

# Check the output
grep -q 'cloudType .low.:' $fTmp
grep -q 'cloudType .high.:' $fTmp

rm -f $fTmp

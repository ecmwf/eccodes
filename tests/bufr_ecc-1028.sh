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
set -u
label="bufr_ecc-1028_test"
tempFilt=temp.$label.filt
tempError=temp.$label.err

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# Incorrect replication. Encoding should fail
echo "set unexpandedDescriptors={ 101000, 31002 }; write;" > $tempFilt
set +e
${tools_dir}/bufr_filter $tempFilt $sample_bufr4 2>$tempError
status=$?
set -e
[ $status -ne 0 ]
# Check error message
grep -q "Delayed replication: 101000: expected 1 but only found 0 elements" $tempError


rm -f $tempFilt $tempError

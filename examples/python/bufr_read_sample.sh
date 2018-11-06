#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh


# Define a common label for all the tmp files
label="bufr_read_sample_test_p"

# Define tmp file
fTmp=${label}".tmp.txt"

REDIRECT=/dev/null

$PYTHON $examples_src/bufr_read_sample.py ${ECCODES_SAMPLES_PATH}/BUFR3.tmpl 2> $REDIRECT
$PYTHON $examples_src/bufr_read_sample.py ${ECCODES_SAMPLES_PATH}/BUFR4.tmpl 2> $REDIRECT

# Clean up
rm -f $fTmp

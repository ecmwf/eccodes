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

#Define a common label for all the tmp files
label="bufr_ecc_448_test_p"

#Define tmp file
fTmp=${label}".tmp.txt"
rm -f $fTmp

REDIRECT=/dev/null

#The bufr file to use 
f=${data_dir}/bufr/syno_1.bufr

#The input ($f) is hardcoded in the example!!!
$PYTHON $examples_src/bufr_ecc-448.py 2> $REDIRECT > $fTmp

#TODO: check the output
grep -q '#6#cloudType->percentConfidence' $fTmp
grep -q '#2#verticalSignificanceSurfaceObservations->percentConfidence' $fTmp

#Clean up
rm -f $fTmp

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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-359.
# It tests calling 'pack=1' and nothing else!
# ---------------------------------------------------------
label="bufr_ecc_359_test"

tempOut=temp.${label}.out
BufrFile=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# This should exit with status 0 and issue an error but not crash
echo 'set pack=1;' | ${tools_dir}/codes_bufr_filter -f - $BufrFile

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
REDIRECT=/dev/null
label="bufr_ecc-1476_test"
temp=temp.$label
sample_bufr3=$ECCODES_SAMPLES_PATH/BUFR3.tmpl
sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

#...
#infile=${data_dir}/SOME_FILE
#${tools_dir}/grib_get
#${tools_dir}/grib_set
#grib_check_key_equals $temp k1,k2 "v1 v2"
#${tools_dir}/bufr_get
#${tools_dir}/bufr_set
#...

rm -f $temp


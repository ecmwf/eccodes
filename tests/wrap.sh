#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

# Tests for WRAP

. ./include.sh
label="wrap-test"
tempOut=temp.${label}.out
tempTxt=temp.${label}.txt
tempRef=temp.${label}.ref

REDIRECT=/dev/null

input=$ECCODES_SAMPLES_PATH/wrap.tmpl

${tools_dir}/grib_dump -TA -O $input
id=`${tools_dir}/grib_get -TA -p identifier $input`
[ "$id" = "WRAP" ]


# Clean up
rm -f $tempOut $tempRef $tempTxt

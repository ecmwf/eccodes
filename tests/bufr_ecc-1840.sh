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

REDIRECT=/dev/null

label="bufr_ecc-1840_test"
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_bufr3=$ECCODES_SAMPLES_PATH/BUFR3.tmpl

# This sample has:
#  typicalYearOfCentury = 12
#  typicalMonth = 10
#  typicalDay = 31
result=$(${tools_dir}/bufr_get -p typicalYear,typicalDate $sample_bufr3)
[ "$result" = "2012 20121031" ]
result=$(${tools_dir}/bufr_get -p typicalYear,typicalDate -s typicalCentury=20 $sample_bufr3)
[ "$result" = "1912 19121031" ]


# Clean up
rm -f $tempBufr $tempFilt $tempLog $tempOut $tempRef

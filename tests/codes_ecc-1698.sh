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

label="codes_ecc-1698_test"
tempGrib=temp.$label.grib
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# Double to string
# ------------------
infile=$sample_grib2
result=$(${tools_dir}/grib_get -p maximum:s $infile)
[ "$result" = "273" ]
result=$(${tools_dir}/grib_get -s formatForDoubles=%e -p maximum:s $infile)
[ "$result" = "2.730000e+02" ]

infile=${data_dir}/sample.grib2
result=$(${tools_dir}/grib_get -s formatForDoubles=%e -p minimum:s $infile)
[ "$result" = "2.704668e+02" ]


# Integer to string
# ------------------
infile=${data_dir}/sample.grib2
result=$(${tools_dir}/grib_get -s formatForLongs=%lX -p year:s $infile)
[ "$result" = "7D8" ]


rm -f $tempGrib $tempBufr $tempFilt $tempLog $tempRef

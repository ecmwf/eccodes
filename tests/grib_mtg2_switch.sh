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

label="grib_mtg2_test"
tempGrib=temp.$label.grib
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

#...
#infile=${data_dir}/SOME_FILE
#cat >$tempFilt<<EOF
#EOF
#${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
#${tools_dir}/grib_get
#${tools_dir}/grib_set
#grib_check_key_equals $temp k1,k2 "v1 v2"

#${tools_dir}/bufr_get
#${tools_dir}/bufr_set
#...

# Clean up
rm -f $tempGrib $tempBufr $tempFilt $tempLog $tempOut $tempRef

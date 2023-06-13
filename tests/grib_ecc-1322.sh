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

label="grib_ecc-1322_test"

tempGrb=temp.$label.grib
tempOut=temp.$label.out
tempRef=temp.$label.ref

sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl

${tools_dir}/grib_set -s edition=2 $sample_grib1 $tempGrb
${tools_dir}/grib_compare -e -b param $sample_grib1 $tempGrb > $tempOut 2>&1

[ ! -s "$tempOut" ]

# Clean up
rm -f $tempGrb $tempOut $tempRef

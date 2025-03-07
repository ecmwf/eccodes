#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

OUT_TMP=out.set_bitmap_f.grib

# The input and output files are hardcoded in the example
${examples_dir}/f_grib_set_bitmap

x=`${tools_dir}/grib_get -p numberOfDataPoints,numberOfCodedValues,numberOfMissing $OUT_TMP`
[ "$x" = "496 486 10" ]

rm -f $OUT_TMP

#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

#Define a common label for all the tmp files
label="grib_copy_namespace_test_f"

INPUT=../../data/regular_latlon_surface.grib1
res=`${tools_dir}/grib_get -p Ni,Nj $INPUT`
[ "$res" = "16 31" ]

# Note: The input and output GRIB files are hardcoded in the example!
# The output is a clone of the regular lat/lon but then
# we copy the reduced lat/lon geography namespace to the output
OUTPUT=temp.$label.grib
${examples_dir}/eccodes_f_grib_copy_namespace

res=`${tools_dir}/grib_get -p Ni,Nj $OUTPUT`
[ "$res" = "MISSING 501" ]

rm -f $OUTPUT

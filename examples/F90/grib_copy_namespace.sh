#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

#Define a common label for all the tmp files
label="grib_copy_namespace_test_f"

INPUT=../../data/regular_latlon_surface.grib1
res=`${tools_dir}/grib_get -p Ni,Nj $INPUT`
[ "$res" = "16 31" ]

#The input and output BUFR files are hardcoded in the f90 example!!!
OUTPUT=out.grib_copy_namespace.grib
${examples_dir}/eccodes_f_grib_copy_namespace

res=`${tools_dir}/grib_get -p Ni,Nj $OUTPUT`
[ "$res" = "MISSING 501" ]

rm -f $OUTPUT

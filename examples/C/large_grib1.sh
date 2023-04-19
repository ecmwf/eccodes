#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

# The executable will produce a GRIB1 file called "bigfile.grib"
output=bigfile.grib
${examples_dir}/c_large_grib1

# Make sure the newly created file can be listed OK
${tools_dir}/grib_ls -p numberOfDataPoints,numberOfCodedValues,numberOfMissing $output
num=$(${tools_dir}/grib_get -p numberOfDataPoints $output)
[ $num = 8294400 ]

rm -f $output

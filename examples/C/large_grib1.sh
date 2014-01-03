#!/bin/sh
# Copyright 2005-2014 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

# The executable should produce a GRIB1 file
${examples_dir}large_grib1

# Make sure it can be listed OK
output=${examples_dir}bigfile.grib
${tools_dir}grib_ls $output > /dev/null
rm -f $output


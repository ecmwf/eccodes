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

# ---------------------------------------------------------
# ECC-899: Using a GRIB index is not thread safe
# ---------------------------------------------------------

label="grib_threads_ecc-899-index"
temp_dir=tempdir.${label}

rm -fr $temp_dir
mkdir -p $temp_dir
cd $temp_dir

$EXEC ${test_dir}/grib_threads_ecc-899-index

# Clean up
cd $test_dir
rm -fr $temp_dir

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

label="grib_ecc-1764_test"
temp=temp.$label.txt

infile=${data_dir}/tigge_cf_ecmwf.grib2
$EXEC ${test_dir}/grib_ecc-1764 $infile # > $temp

# Clean up
rm -f $temp

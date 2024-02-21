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

label="grib_clone_headers_only_test"
temp=temp.$label.grib

inputs="
  sst_globus0083.grib
  sample.grib2
  mixed.grib
"

for f in $inputs; do
    infile=$data_dir/$f
    rm -f $temp
    $EXEC ${test_dir}/grib_clone_headers_only $infile $temp
    ${tools_dir}/grib_compare -H -b totalLength $infile $temp
done

# Clean up
rm -f $temp

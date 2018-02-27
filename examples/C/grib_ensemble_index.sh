#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

input="../../data/index.grib"
${examples_dir}/c_grib_ensemble_index $input

# ECC-378: Empty input file
temp=temp_index.empty.grib
touch $temp
set +e
${examples_dir}/c_grib_ensemble_index $temp 2>/dev/null
status=$?
set -e
[ $status -ne 0 ]
rm -f $temp

#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

# GitHub issue #548: GRIB1 stepType=min must not decode as max
. ./include.ctest.sh

label="grib_step_type_minmax_test"
temp=temp.$label.grib
sample1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl

for st in min max accum avg; do
  ${tools_dir}/grib_set -s stepType=$st,stepRange=2-6 $sample1 $temp
  grib_check_key_equals $temp stepType,stepRange "$st 2-6"
done

rm -f $temp

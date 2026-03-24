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

label="grib_merge_test"

tempGrib=temp.$label.grib

f1=$ECCODES_SAMPLES_PATH/regular_ll_pl_grib1.tmpl
f2=$ECCODES_SAMPLES_PATH/regular_ll_sfc_grib1.tmpl
${tools_dir}/grib_merge $f1 $f2 $tempGrib

# Clean up
rm -f $tempGrib

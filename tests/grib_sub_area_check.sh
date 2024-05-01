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

label="grib_sub_area_check_test"
tempOut=temp.$label.txt

if [ ! -x "${tools_dir}/gg_sub_area_check" ]; then
    exit 0
fi

f1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
f2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/gg_sub_area_check $f1 $f2 > $tempOut

f1=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib1.tmpl
f2=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
${tools_dir}/gg_sub_area_check $f1 $f2 > $tempOut

# Clean up
rm -f $tempOut

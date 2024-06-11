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

label="grib_histogram_test"
REDIRECT=/dev/null
tempOut=temp.$label.txt

${tools_dir}/grib_histogram $ECCODES_SAMPLES_PATH/GRIB1.tmpl > $tempOut
${tools_dir}/grib_histogram $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut

${tools_dir}/grib_histogram -p step,shortName $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut

# Something with a bitmap
input=$data_dir/reduced_latlon_surface.grib2
grib_check_key_equals $input numberOfMissing 98701
${tools_dir}/grib_histogram $input > $tempOut

# Skip
input=$data_dir/tigge_cf_ecmwf.grib2
${tools_dir}/grib_histogram -w level=925,shortName=gh $input > $tempOut


# Clean up
rm -f $tempOut

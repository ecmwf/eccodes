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

label="grib_to_ppm_test"
tempOut=temp.$label.txt

if [ ! -x "${tools_dir}/grib2ppm" ]; then
    exit 0
fi

${tools_dir}/grib2ppm $ECCODES_SAMPLES_PATH/GRIB1.tmpl > $tempOut
${tools_dir}/grib2ppm $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut

# Clean up
rm -f $tempOut

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

label="grib_list_keys_test"
tempOut=temp.$label.txt

if [ -d "$ECCODES_DEFINITION_PATH" ]; then
    if [ -x "${tools_dir}/grib_list_keys" ]; then
        ${tools_dir}/grib_list_keys $ECCODES_DEFINITION_PATH/boot.def > $tempOut
        ${tools_dir}/grib_list_keys $ECCODES_DEFINITION_PATH/grib2/template.4.2000.def > $tempOut
        ${tools_dir}/grib_list_keys $ECCODES_DEFINITION_PATH/grib2/template.3.0.def > $tempOut
        ${tools_dir}/grib_list_keys $ECCODES_DEFINITION_PATH/grib2/template.3.120.def > $tempOut
    fi
fi

# Clean up
rm -f $tempOut

#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

label="grib_unstructured_grid"

# Only relevant for GRIB2
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
temp=temp.${label}

${tools_dir}/grib_set -s gridType=unstructured_grid,numberOfGridInReference=4,numberOfGridUsed=2 $sample $temp
grib_check_key_equals $temp 'unstructuredGridType'    'ORCA1'
grib_check_key_equals $temp 'unstructuredGridSubtype' 'W grid'
#grib_check_key_equals $temp 'unstructuredGridUUID' 'ORCA1 W grid unknown'

rm -f $temp

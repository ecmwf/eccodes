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

label="grib_grid_unstructured"

# Only relevant for GRIB2
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
temp1=temp.1.${label}
temp2=temp.2.${label}

# Default
${tools_dir}/grib_set -s gridType=unstructured_grid $sample $temp1
grib_check_key_equals $temp1 gridName 'undefined'

# Unknown values
${tools_dir}/grib_set -s gridType=unstructured_grid,numberOfGridInReference=94,numberOfGridUsed=200 $sample $temp1
grib_check_key_equals $temp1 gridName 'unknown'

# Two invocations of grib_set
${tools_dir}/grib_set -s gridType=unstructured_grid $sample $temp1
${tools_dir}/grib_set -s unstructuredGridType=ORCA1 $temp1  $temp2

# ORCA1, W grid
${tools_dir}/grib_set -s gridType=unstructured_grid,numberOfGridInReference=4,numberOfGridUsed=2 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridType'    'ORCA1'
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'W'
grib_check_key_equals $temp1 gridName 'ORCA1_W'
# ECC-1183
${tools_dir}/grib_ls -j $temp1 > $temp2
grep -q "gridName.*ORCA1_W" $temp2

${tools_dir}/grib_set -s gridType=unstructured_grid,unstructuredGridType=ORCA1,unstructuredGridSubtype=W $sample $temp2
${tools_dir}/grib_compare $temp1 $temp2
grib_check_key_equals $temp2 'numberOfGridInReference,numberOfGridUsed,gridName' '4 2 ORCA1_W'

# Test bad values
# ---------------
${tools_dir}/grib_set -s gridType=unstructured_grid $sample $temp1
set +e
${tools_dir}/grib_set -s unstructuredGridType=bad $temp1 $temp2 2>/dev/null
status1=$?
${tools_dir}/grib_set -s unstructuredGridSubtype=sad $temp1 $temp2 2>/dev/null
status2=$?
set -e
[ $status1 -ne 0 ]
[ $status2 -ne 0 ]

# Test some lat/lon parameters
# -----------------------------
${tools_dir}/grib_set -s paramId=250003 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'T'

${tools_dir}/grib_set -s paramId=250005 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'U'

${tools_dir}/grib_set -s paramId=250007 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'V'

${tools_dir}/grib_set -s paramId=250009 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'W'

${tools_dir}/grib_set -s paramId=250011 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'F'


rm -f $temp1 $temp2

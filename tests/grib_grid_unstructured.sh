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

${tools_dir}/grib_set -s gridType=unstructured_grid,numberOfGridInReference=4,numberOfGridUsed=2 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridType'    'ORCA1'
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'W grid'
#grib_check_key_equals $temp1 'unstructuredGridUUID' 'ORCA1 W grid unknown'

${tools_dir}/grib_set -s gridType=unstructured_grid,unstructuredGridType=ORCA1,unstructuredGridSubtype='W grid' $sample $temp2
${tools_dir}/grib_compare $temp1 $temp2
grib_check_key_equals $temp2 'numberOfGridInReference' '4'
grib_check_key_equals $temp2 'numberOfGridUsed' '2'

# Test some lat/lon parameters
${tools_dir}/grib_set -s paramId=250003 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'T grid'

${tools_dir}/grib_set -s paramId=250005 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'U grid'

${tools_dir}/grib_set -s paramId=250007 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'V grid'

${tools_dir}/grib_set -s paramId=250009 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'W grid'

${tools_dir}/grib_set -s paramId=250011 $sample $temp1
grib_check_key_equals $temp1 'unstructuredGridSubtype' 'F grid'


rm -f $temp1 $temp2

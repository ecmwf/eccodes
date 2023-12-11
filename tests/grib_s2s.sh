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

grib2_sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
label=grib_s2s_test
tempSample=tempSample.${label}.grib2
temp1=temp1.${label}.grib2
temp2=temp2.${label}.grib2


${tools_dir}/grib_set -s tablesVersion=14,productionStatusOfProcessedData=6 \
                      $grib2_sample $tempSample


# GRIB-761. For Italy, subCentre 102 is ISAC-CNR
# ----------------------------------------------
${tools_dir}/grib_set -s centre=cnmc,subCentre=102 $tempSample $temp1
grib_check_key_equals $temp1 mars.origin 'isac'


# ECC-1428. Valid and invalid mars types
# ---------------------------------------
${tools_dir}/grib_set -s marsType=fc $tempSample $temp1
${tools_dir}/grib_set -s marsType=cf $tempSample $temp1
${tools_dir}/grib_set -s marsType=pf $tempSample $temp1

set +e
${tools_dir}/grib_set -s marsType=xx $tempSample $temp1
status=$?
set -e
[ $status -ne 0 ]

# Reforecasts
${tools_dir}/grib_set -s \
   tablesVersion=14,productionStatusOfProcessedData=6,productDefinitionTemplateNumber=60 \
   $grib2_sample $temp1
${tools_dir}/grib_ls -jm $temp1
grib_check_key_equals $temp1 mars.hdate '20070323'
grib_check_key_equals $temp1 mars.stream 'enfh'
grib_check_key_equals $temp1 isHindcast '1'

# Clean up
rm -f $temp1 $temp2 $tempSample

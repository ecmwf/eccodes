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

label="grib_ecc-1271_test"
temp=temp.$label

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl


${tools_dir}/grib_set -s \
  centre=kwbc,gridDefinitionTemplateNumber=32769,localTablesVersion=1 \
$sample_grib2 $temp


grib_check_key_equals $temp Ni,Nj "16 31"
grib_check_key_equals $temp centreLatitudeInDegrees,centreLongitudeInDegrees "0 30"
grib_check_key_equals $temp minimum,maximum "1 1"

rm -f $temp

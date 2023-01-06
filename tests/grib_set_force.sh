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

label="grib_set_force_test"

tempInput=temp.$label.input.grib2
tempOut1=temp.$label.full.grib2
tempOut2=temp.$label.part.grib2

${tools_dir}/grib_copy -w count=14 ${data_dir}/tigge_af_ecmwf.grib2 $tempInput
${test_dir}/grib_set_force all_values   $tempInput $tempOut1
${test_dir}/grib_set_force coded_values $tempInput $tempOut2

grib_check_key_equals $tempInput numberOfDataPoints,numberOfCodedValues,numberOfMissing '684 237 447'
grib_check_key_equals $tempOut1  numberOfDataPoints,numberOfCodedValues,numberOfMissing '684 237 447'
grib_check_key_equals $tempOut2  numberOfDataPoints,numberOfCodedValues,numberOfMissing '684 237 447'

${tools_dir}/grib_compare $tempOut1 $tempOut2

rm -f $tempInput $tempOut1 $tempOut2

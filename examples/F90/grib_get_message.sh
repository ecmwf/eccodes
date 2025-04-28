#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label="examples_f_grib_get_message"

INPUT="../../data/constant_field.grib1"
OUTPUT1=temp.$label.1.grib1
OUTPUT2=temp.$label.2.grib1

${examples_dir}/f_grib_get_message
${tools_dir}/grib_compare $OUTPUT1 $OUTPUT2

rm -f $OUTPUT1 $OUTPUT2

#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

INPUT="../../data/constant_field.grib1"
OUTPUT=out.copy.grib1

${examples_dir}/eccodes_f_grib_copy_message > /dev/null
${tools_dir}/grib_compare -b centre $INPUT $OUTPUT

rm -f $OUTPUT

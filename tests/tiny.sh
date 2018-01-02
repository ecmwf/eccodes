#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#


. ./include.sh

REDIRECT=/dev/null

cat >${data_dir}/f.rules <<EOF
set values = { 1.0e-110, 1.5e-110, 1.005e-110 }
write;
EOF

${tools_dir}/grib_filter ${data_dir}/f.rules ${data_dir}/ 2> $REDIRECT > $REDIRECT


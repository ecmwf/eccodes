#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

files="regular_latlon_surface.grib2 regular_latlon_surface.grib1"

for file in $files
do

cat >statistics.filter<<EOF
set Ni=2;
set Nj=2;

set decimalPrecision=4;
set values={2.0,2.0,2.0,2.0};
print "values=[values]";
print "max=[max] min=[min] average=[average]";

set values={2.0,5.0,2.0,2.0};
print "values=[values]";
print "max=[max] min=[min] average=[average]";

EOF

${tools_dir}grib_filter statistics.filter ${data_dir}/$file > statistics.out

diff statistics.out ${data_dir}/statistics.out.good

done

rm -f statistics.out statistics.filter || true

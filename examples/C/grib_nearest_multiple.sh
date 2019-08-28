#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

label="grib_nearest_multiple"
temp1=$label.temp1.$$
temp2=$label.temp2.$$
tempRef=$label.ref.$$

cat > $temp1 <<EOF
1 30 -20
2 13 234
EOF

input_pts=$temp1
input_lsm=${data_dir}/reduced_gaussian_lsm.grib1
input_grb=${data_dir}/reduced_gaussian_pressure_level.grib1

${examples_dir}/c_grib_nearest_multiple $input_pts $input_lsm $input_grb > $temp2

cat > $tempRef <<EOF
1 29.30 340.31 0.00 289.87 1770
2 12.56 233.44 0.00 294.06 2500
EOF

diff $tempRef $temp2


rm -f $temp1 $temp2 $tempRef

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

${tools_dir}/grib_filter statistics.filter ${data_dir}/$file > statistics.out

diff statistics.out ${data_dir}/statistics.out.good

done
rm -f statistics.out statistics.filter


# GRIB with no missing values but some entries = 9999
# See ECC-478
# ---------------------------------------------------
input=${data_dir}/lfpw.grib1
temp1=temp1.statistics.grib
temp2=temp2.statistics.grib
stats=`${tools_dir}/grib_get -w count=50 -F%.2f -n statistics $input`
[ "$stats" = "10098 0 1064.19 3066.07 2.57004 4.60965 0" ]

# Scaling values in presence of real 9999 values
${tools_dir}/grib_set -s scaleValuesBy=0.5                     $input $temp1
${tools_dir}/grib_set -s missingValue=1.0E34,scaleValuesBy=0.5 $input $temp2
${tools_dir}/grib_compare $temp1 $temp2

# Offsetting values in presence of real 9999 values
${tools_dir}/grib_set -s offsetValuesBy=0.5                     $input $temp1
${tools_dir}/grib_set -s missingValue=1.0E34,offsetValuesBy=0.5 $input $temp2
${tools_dir}/grib_compare $temp1 $temp2

# ECC-511
# GRIB2 message from NCEP/GFS with grid_complex_spatial_differencing and
# missingValueManagementUsed. No bitmap but missing values embedded in data
input=${data_dir}/gfs.complex.mvmu.grib2
stats=`${tools_dir}/grib_get -F%.2f -p max,min,avg $input`
[ "$stats" = "2.81 0.00 0.30" ]

rm -f $temp1 $temp2

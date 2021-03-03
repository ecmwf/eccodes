#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh
temp=temp.c_grib_iterator.txt

# These two do not have any missing data
${examples_dir}/c_grib_iterator ${data_dir}/reduced_gaussian_model_level.grib1 > /dev/null
${examples_dir}/c_grib_iterator ${data_dir}/regular_gaussian_model_level.grib1 > /dev/null

# Has missing data
${examples_dir}/c_grib_iterator ${data_dir}/reduced_latlon_surface.grib2 > $temp
count_miss=`grep -c missing $temp`
[ $count_miss = 98701 ]

rm -f $temp

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

label="grib_units_bias_factor_test"
outfile=temp.$label.grib

infile=${data_dir}/sample.grib2
max=`${tools_dir}/grib_get -F%.3f -p max $infile`
[ "$max" = "311.099" ]

${tools_dir}/grib_set -rs unitsFactor=1.1,unitsBias=4 $infile $outfile
max=`${tools_dir}/grib_get -F%.3f -p max $outfile`
[ "$max" = "346.209" ]

${tools_dir}/grib_set -rs unitsFactor=2 $infile $outfile
max=`${tools_dir}/grib_get -F%.3f -p max $outfile`
[ "$max" = "622.197" ]

${tools_dir}/grib_set -rs unitsBias=4 $infile $outfile
max=`${tools_dir}/grib_get -F%.3f -p max $outfile`
[ "$max" = "315.099" ]


# Clean up
rm -f $outfile

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

label="grib_g1day_of_the_year_date"
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
temp=temp.$label.grib

${tools_dir}/grib_set -s marsType=s3,marsStream=mpic $sample_g1 $temp
grib_check_key_equals $temp "mars.date:s,dayOfTheYearDate:s" "2006-076 2006-076"

${tools_dir}/grib_set -s marsType=s3,marsStream=ukmo $sample_g1 $temp
grib_check_key_equals $temp "mars.date:s,dayOfTheYearDate:s" "2006-076 2006-076"


# Clean up
rm -f $temp

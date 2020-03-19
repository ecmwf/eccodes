#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

label="grib_g1fcperiod"
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
temp=temp.$label.grib

${tools_dir}/grib_set -s \
  localDefinitionNumber=23,P1=12,P2=18,marsType=fcmin,marsStream=mfwm,timeRangeIndicator=3,unitOfTimeRange=2 \
$sample_g1 $temp

grib_check_key_equals $temp "mars.fcperiod,forecastperiod" "12-18 12-18"

# Clean up
rm -f $temp

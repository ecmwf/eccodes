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

label="grib_local_NCEP_test"
temp=temp.$label.grib
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s centre=kwbc,discipline=3,parameterCategory=192,parameterNumber=35 $sample $temp

grib_check_key_equals $temp paramId,shortName '7001339 SRFA176'

rm -f $temp

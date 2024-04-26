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

REDIRECT=/dev/null

label="grib_ecc-1440_test"
tempGrib=temp.$label.grib
tempLog=temp.$label.log

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

set +e
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=300 $sample_grib2 $tempGrib > $tempLog 2>&1
status=$?
set -e
grep -q "This local definition has been deprecated" $tempLog

rm -f $tempGrib $tempLog

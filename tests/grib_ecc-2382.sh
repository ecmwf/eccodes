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

# ---------------------------------------------------------
# This is the test for JIRA issue ECC-2382
# < shortNames for temperature anomalies >
# ---------------------------------------------------------

REDIRECT=/dev/null

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib=temp.$label.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s tablesVersion=37,paramId=171130 $sample_grib2 $tempGrib

grib_check_key_equals $tempGrib paramId,shortName "171130 ta"

${tools_dir}/grib_set -s tablesVersion=37,paramId=3025 $sample_grib2 $tempGrib

grib_check_key_equals $tempGrib paramId,shortName "3025 t_a"

# Clean up
rm -f $tempGrib


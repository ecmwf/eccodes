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
# This is the test for JIRA issue ECC-XXXX
# < Add issue summary here >
# ---------------------------------------------------------

if [ ${HAVE_ECKIT_GEO} -ne 1 ]
then
    echo "$0: This test is disabled when HAVE_ECKIT_GEO=OFF"
    exit 0
fi

if [ "${ECCODES_ECKIT_GEO:-0}" -eq 0 ]
then
    echo "$0: This test is disabled (env. variable ECCODES_ECKIT_GEO=0)"
    exit 0
fi

REDIRECT=/dev/null

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib=temp.$label.grib
sample_grib2=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl

${tools_dir}/grib_set -s year=1983,gridSpec='{area:[20,0,0,20],grid:[10,10]}',month=1 ${sample_grib2} $tempGrib
# check return code
if [ $? -ne 0 ]; then
    echo "grib_set failed"
    exit 1
fi

grib_check_key_equals $tempGrib month 1
grib_check_key_equals $tempGrib year 1983
grib_check_key_equals $tempGrib gridSpec '{"area":[20,0,0,20],"grid":[10,10]}'

# Clean up
rm -f $tempGrib

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
# This is the test for JIRA issue ECC-2374
# < SMOS local parameters >
# ---------------------------------------------------------

REDIRECT=/dev/null

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat >$tempFilt<<EOF
set centre=98;
set tablesVersion=37;
set discipline=254;
set parameterCategory=252;
set parameterNumber=192;
set localTablesVersion=1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2

result=$(${tools_dir}/grib_get -p paramId $tempGrib)
[ "$result" = "228070" ]

result=$(${tools_dir}/grib_get -p shortName $tempGrib)
[ "$result" = "smnnob" ]

# Clean up
rm -f $tempGrib $tempFilt

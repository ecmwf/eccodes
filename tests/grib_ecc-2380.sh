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
# This is the test for JIRA issue ECC-2380
# GRIB2: Hydro fu and sfo types
# ---------------------------------------------------------

REDIRECT=/dev/null

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# SFO case, use offsetToStartOfAnalysisInHours as addition to date
cat >$tempFilt<<EOF
set setLocalDefinition = 1;
set grib2LocalSectionNumber = 44;
set marsClass = 42;
set marsType = 73;
set marsStream = 1025;
set offsetToStartOfAnalysisInHours = 96;
set offsetToStartOfForecastFillupInHours = 24;
set productDefinitionTemplateNumber = 72;
set significanceOfReferenceTime = 1;
set year = 2000;
set month = 1;
set day = 1;
set hour = 12;
set minute = 0;
set second = 0;
set inputProcessIdentifier = 65533;
set inputOriginatingCentre = 98;
set backgroundProcess = 147;
set generatingProcessIdentifier = 42;
write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2

result=$(${tools_dir}/grib_get -p mars.date $tempGrib)
[ "$result" = "20000105" ]

# FU case, use offsetToStartOfForecastFillupInHours as addition to date
cat >$tempFilt<<EOF
set setLocalDefinition = 1;
set grib2LocalSectionNumber = 44;
set marsClass = 42;
set marsType = 72;
set marsStream = 1025;
set offsetToStartOfAnalysisInHours = 96;
set offsetToStartOfForecastFillupInHours = 24;
set productDefinitionTemplateNumber = 72;
set significanceOfReferenceTime = 1;
set year = 2000;
set month = 1;
set day = 1;
set hour = 12;
set minute = 0;
set second = 0;
set inputProcessIdentifier = 65533;
set inputOriginatingCentre = 98;
set backgroundProcess = 147;
set generatingProcessIdentifier = 42;
write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2

result=$(${tools_dir}/grib_get -p mars.date $tempGrib)
[ "$result" = "20000102" ]

# Clean up
rm -f $tempGrib $tempFilt


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
# This is the test for JIRA issue ECC-2221
# Hydrological CEMS and C3S data refactoring of the encoding and the archival
# ---------------------------------------------------------

label="grib_ecc-2221_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set class="ef";
    set tablesVersion=35;
    set productDefinitionTemplateNumber=73;
    set inputProcessIdentifier=16;
    set inputOriginatingCentre=98;
    set backgroundProcess=147;
    set generatingProcessIdentifier=5;
    set indicatorOfUnitForTimeRange=1;
    set lengthOfTimeRange=6;
    set paramId=235270;
    write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib model,configuration,forcing,timespan "lisflood v5 ecmf-ifs 6h"

# Clean up
rm -f $tempGrib $tempFilt

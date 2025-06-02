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
# This is the test for the JIRA issue ECC-1785
# BUFR: Allow encoding satelliteID when rdbType=30
# ---------------------------------------------------------

label="bufr_ecc-1785_test"

tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempLog=temp.$label.log

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR3_local_satellite.tmpl

echo 'set numberOfSubsets=1; set rdbType=30; set satelliteID=78; write;' > $tempFilt

${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $sample_bufr4

result=$(${tools_dir}/bufr_get -p isSatelliteType,isSatellite,satelliteID $tempBufr)
[ "$result" = "1 1 78" ]

# Clean up
rm -f $tempBufr $tempFilt $tempLog

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

# ------------------------------------------------------------
# This is the test for the JIRA issue ECC-1941
# Setting paramId=131060 fails if typeOfFirstFixedSurface=255
# ------------------------------------------------------------

label="grib_ecc-1941_test"
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s paramId=131060 $sample_grib2 $tempGribA
grib_check_key_equals $tempGribA paramId,shortName '131060 tpg1'

${tools_dir}/grib_set -s typeOfFirstFixedSurface=255 $sample_grib2 $tempGribA
grib_check_key_equals $tempGribA typeOfLevel unknown

${tools_dir}/grib_set -s paramId=131060 $tempGribA $tempGribB
grib_check_key_equals $tempGribB paramId,shortName '131060 tpg1'

# Clean up
rm -f $tempGribA $tempGribB

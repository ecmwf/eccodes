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
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1167
# GRIB: Allow setting byte array as string
# ---------------------------------------------------------
label="grib_ecc-1167-test"
temp1=temp.${label}.1.grib
temp2=temp.${label}.2.grib
errlog=temp.${label}.err

sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s gridType=unstructured_grid $sample2 $temp1
grib_check_key_equals $temp1 uuidOfHGrid '00000000000000000000000000000000'

${tools_dir}/grib_set -s uuidOfHGrid=DEfdBEef10203040b00b1e50001100FF $temp1 $temp2
grib_check_key_equals $temp2 uuidOfHGrid 'defdbeef10203040b00b1e50001100ff'

${tools_dir}/grib_set -s uuidOfHGrid:s=10991111111111111111115000110000 $temp1 $temp2
grib_check_key_equals $temp2 uuidOfHGrid '10991111111111111111115000110000'


# Test errors
set +e
# Invalid HEX
${tools_dir}/grib_set -s uuidOfHGrid=DEZZBEef10203040b00b1e50001100FF $temp1 $temp2 2>$errlog
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid hex byte specfication 'ZZ'" $errlog


# Clean up
rm -f $temp1 $temp2 $errlog

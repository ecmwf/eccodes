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
# This is the test for the JIRA issue ECC-2030.
# bufr_dump should fail if unknown descriptor
# ---------------------------------------------------------

label="bufr_ecc-2030_test"

tempBufrA=temp.$label.A.bufr
tempBufrB=temp.$label.B.bufr
tempFilt=temp.$label.filt
tempLog=temp.$label.log

sample=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

# Descriptor 1104 exists in version 29 but not in version 20
echo 'set masterTablesVersionNumber=29;set unexpandedDescriptors={1104};write;' | ${tools_dir}/codes_bufr_filter -o $tempBufrA - $sample
${tools_dir}/bufr_dump -p $tempBufrA

# Now it should fail to decode
${tools_dir}/bufr_set -s masterTablesVersionNumber=20 $tempBufrA $tempBufrB
set +e
${tools_dir}/bufr_dump -p $tempBufrB 2>$tempLog
status=$?
set -e
[ $status -ne 0 ]
grep -q "unable to get descriptor 001104 from table" $tempLog


# Clean up
rm -f $tempBufrA $tempBufrB $tempFilt $tempLog

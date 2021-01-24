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

# Define a common label for all the tmp files
label="bufr_dump_subset_test"

# Create log file
fLog=${label}".log"
rm -f $fLog

# Define tmp bufr files
temp1=${label}".temp1.txt"
temp2=${label}".temp2.txt"

if [ $HAVE_MEMFS -eq 1 ]; then
    unset ECCODES_DEFINITION_PATH
    unset ECCODES_SAMPLES_PATH
fi

test_expected_failure()
{
    bfile=$1
    barg=$2
    set +e
    ${tools_dir}/bufr_dump -p -S $barg $bfile
    status=$?
    set -e
    [ $status -ne 0 ]
}

# This has 128 subsets
input=${data_dir}/bufr/aaen_55.bufr
test_expected_failure $input nan
test_expected_failure $input -1
test_expected_failure $input 0
test_expected_failure $input 1000

# Dump of BUFR with one subset should be the same as original
input=${data_dir}/bufr/amda_144.bufr
${tools_dir}/bufr_dump -S1 $input > $temp1
${tools_dir}/bufr_dump     $input > $temp2
diff $temp1 $temp2

input=${data_dir}/bufr/synop_multi_subset.bufr
${tools_dir}/bufr_dump -p -S1 $input | grep 'stationOrSiteName="TROMSO-HOLT"'
${tools_dir}/bufr_dump -p -S12 $input| grep 'stationOrSiteName="SANDE-GALLEBERG"'

#==============================================
# Test all downloaded BUFR files
#==============================================
bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
for file in ${bufr_files}
do
  # Every file has at least one subset
  ${tools_dir}/bufr_dump -wcount=1 -S1 ${data_dir}/bufr/$file >/dev/null
done

# Check strict option with 'where' clause
input=${data_dir}/bufr/tropical_cyclone.bufr
ce=`${tools_dir}/bufr_dump -p -w count=3 $input | grep -c 'edition='`
[ $ce -eq 1 ]

# Clean up
rm -f $temp1 $temp2

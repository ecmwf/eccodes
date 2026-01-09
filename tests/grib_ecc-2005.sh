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

# ---------------------------------------------------------------------
# This is the test for the JIRA issue ECC-2005
# Setting of hdate and number do not show immediately in mars namespace
# ---------------------------------------------------------------------

label="grib_ecc-2005_test"

tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

# Add local section in memory to ensure
# mars key 'hdate' appears
infile=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
cat >$tempFilt<<EOF
    set setLocalDefinition = 1;
    set stream = "eefh";
    set type = "cf";
    print "[mars.hdate]";
    assert( hdate == dataDate );
EOF
${tools_dir}/grib_filter $tempFilt $infile

# Once bug is fully fixed, remove all the 'set +e'
run_exec=1
if [ $run_exec -eq 1 ]; then
    # Try with a GRIB1 sample with mars keys
    infile=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
    set +e
    $EXEC $test_dir/grib_ecc-2005 $infile >$tempLog 2>&1
    status=$?
    set -e
    [ $status -ne 0 ]
    cat $tempLog
    grep -q "Key 'hdate' was not in the mars namespace" $tempLog

    # Try with a GRIB2 sample without a local section (no mars keys)
    set +e
    infile=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
    $EXEC $test_dir/grib_ecc-2005 $infile
    status=$?
    set -e
    [ $status -ne 0 ]

    # Try with a GRIB2 sample with a local section (has mars keys)
    set +e
    infile=$ECCODES_SAMPLES_PATH/destine_grib2.tmpl
    grib_check_key_equals $infile stream,type 'oper an'
    $EXEC $test_dir/grib_ecc-2005 $infile
    status=$?
    set -e
    [ $status -ne 0 ]
fi

# Clean up
rm -f $tempGrib $tempFilt $tempLog $tempOut $tempRef

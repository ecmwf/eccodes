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
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-984
# grib_get_data: missing jDirectionIncrement produces huge values
# ---------------------------------------------------------
label="grib_ecc-984_test"
tempGrb=temp.${label}.grb
tempOut=temp.${label}.txt
tempErr=temp.${label}.err

input=$data_dir/sample.grib2
${tools_dir}/grib_set -s ijDirectionIncrementGiven=0 $input $tempGrb
grib_check_key_equals $tempGrb jDirectionIncrementInDegrees MISSING
${tools_dir}/grib_get_data -F%.2f $tempGrb >$tempOut 2>$tempErr

grep -q "2.000   20.000 302.53"  $tempOut
# grep -q "Cannot use jDirectionIncrement" $tempErr

# ECC-1586
${tools_dir}/grib_set -s jDirectionIncrement=MISSING,iDirectionIncrement=MISSING $input $tempGrb
${tools_dir}/grib_get_data -F%.2f $tempGrb >$tempOut 2>$tempErr
grep -q "2.000   20.000 302.53"  $tempOut


# Clean up
rm -f $tempGrb $tempOut $tempErr

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
REDIRECT=/dev/null
label="bufr_ecc-1195-test"
temp1=temp.$label.bufr
temp2=temp.$label.txt
sample_bufr3=$ECCODES_SAMPLES_PATH/BUFR3_local.tmpl
sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4_local.tmpl

should_fail_setting()
{
    _keyvals=$1
    _sample=$2
    set +e
    ${tools_dir}/bufr_set -s $_keyvals $_sample $temp1 2>$temp2
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "Value is read only" $temp2
}

should_fail_setting localDate=20140102    $sample_bufr3
should_fail_setting localTime=1010        $sample_bufr3
should_fail_setting rdbtimeDate=20121030  $sample_bufr3
should_fail_setting rdbtimeTime=001018    $sample_bufr3
should_fail_setting typicalDate=20121030  $sample_bufr3
should_fail_setting typicalTime=000001    $sample_bufr3

should_fail_setting localDate=20140102    $sample_bufr4
should_fail_setting localTime=1010        $sample_bufr4
should_fail_setting rdbtimeDate=20121030  $sample_bufr4
should_fail_setting rdbtimeTime=001018    $sample_bufr4
should_fail_setting typicalDate=20121030  $sample_bufr4
should_fail_setting typicalTime=000001    $sample_bufr4


rm -f $temp1 $temp2

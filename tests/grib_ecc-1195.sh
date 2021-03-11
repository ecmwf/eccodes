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
label="grib_ecc-1195-test"
temp1=temp.$label.grib
temp2=temp.$label.txt
sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4_local.tmpl


${tools_dir}/grib_set -s localDefinitionNumber=3 $sample_grib1 $temp1
set +e
${tools_dir}/grib_set -s marsIdent=0 $temp1 /dev/null 2>$temp2
status=$?
set -e
[ $status -ne 0 ]
grep -q "Value is read only" $temp2

set +e
${tools_dir}/grib_set -s indicatorOfTypeOfLevel=101,levels=1-1 $sample_grib1 $temp1 2>$temp2
status=$?
set -e
[ $status -ne 0 ]
grep -q "Value is read only" $temp2

#set +e
#${tools_dir}/bufr_set -s localDate=20140102 $sample_bufr4 $temp1 2>$temp2
#status=$?
#set -e
#[ $status -ne 0 ]
#grep -q "Value is read only" $temp2


rm -f $temp1 $temp2

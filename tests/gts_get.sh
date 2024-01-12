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

#Enter data dir
cd ${data_dir}/gts

label="gts_get_test"

fLog=${label}".log"
rm -f $fLog

fTmp=${label}".tmp.txt"

#----------------------------------------------
# Test "-p" switch
#----------------------------------------------
gts_file=EGRR20150317121020_00493212.DAT
${tools_dir}/gts_get -p TT,AA,II,CCCC,YY,GG,gg,BBB $gts_file >/dev/null

#----------------------------------------------
# Test "-w" switch
#----------------------------------------------
${tools_dir}/gts_get -p TT -w count=3 $gts_file


#----------------------------------------------
# Test "-s" switch
#----------------------------------------------
result=$( ${tools_dir}/gts_get -s YY=ab -p YY -w count=3 $gts_file )
[ "$result" = "ab" ]


gts_file=${data_dir}/gts.grib
result=$( ${tools_dir}/grib_get -wcount=1 -p gts_CCCC -g $gts_file )
[ "$result" = "ECMG" ]

${tools_dir}/grib_get -wcount=1 -p gts_header -g $gts_file

# Encoding should be disabled
# -----------------------------
set +e
${tools_dir}/grib_set -s gts_CCCC=xxx -g $gts_file $fTmp
status=$?
set -e
[ $status -ne 0 ]

set +e
${tools_dir}/grib_set -s gts_header=yyy -g $gts_file $fTmp
status=$?
set -e
[ $status -ne 0 ]


# Clean up
rm -f $fLog $fTmp

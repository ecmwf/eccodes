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
cd ${data_dir}/metar

#Define a common label for all the tmp files
label="metar_dump_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp file
fTmp=${label}".tmp.txt"
rm -f $fTmp

#----------------------------------------------
# Test default "dump" on all the metar data files
#----------------------------------------------
export METAR_YEAR=2015
export METAR_MONTH=4
metar_file=metar.txt
f=$metar_file

echo $f >> $fLog
${tools_dir}/metar_dump -w count=1 $f >> $fLog
${tools_dir}/metar_dump -Dat $f >> $fLog
${tools_dir}/metar_dump -OH  $f >> $fLog

ref_dump=$f".dump.ref"
res_dump=$f".dump.test"
REDIRECT=/dev/null

${tools_dir}/metar_dump $f 2> $REDIRECT > $res_dump

diff $ref_dump $res_dump >$REDIRECT 2> $REDIRECT

# Data
${tools_dir}/metar_dump -d $f

# JSON
${tools_dir}/metar_dump -j $f

# Skip
${tools_dir}/metar_dump -w count=11 $f

# Clean up
rm -f $fLog $res_dump 

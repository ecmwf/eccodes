#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

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
${tools_dir}/metar_dump $f >> $fLog

#-------------------------------------------
# Test "-p" switch
#-------------------------------------------
ref_dump=$f".dump.ref"
res_dump=$f".dump.test"
REDIRECT=/dev/null

${tools_dir}/metar_dump $f 2> $REDIRECT > $res_dump

diff $ref_dump $res_dump >$REDIRECT 2> $REDIRECT

rm -f $fLog $res_dump 

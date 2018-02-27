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
label="metar_ls_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp file
fTmp=${label}".tmp.txt"
rm -f $fTmp

#----------------------------------------------
# Test default "ls" on all the metar data files
#----------------------------------------------
metar_file=metar.txt
f=$metar_file
export METAR_YEAR=2015
export METAR_MONTH=4

echo $f >> $fLog
${tools_dir}/metar_ls $f >> $fLog

#-------------------------------------------
# Test "-p" switch
#-------------------------------------------
ref_ls=$f".ls.ref"
res_ls=$f".ls.test"
REDIRECT=/dev/null

${tools_dir}//metar_ls -pCCCC,latitude,longitude,dateTime,elevation,temperature,dewPointTemperature,qnh $f 2> $REDIRECT > $res_ls

diff $ref_ls $res_ls >$REDIRECT 2> $REDIRECT

rm -f $fLog $res_ls 

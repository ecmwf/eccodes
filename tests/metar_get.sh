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
label="metar_get_test"

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
export METAR_YEAR=2015
export METAR_MONTH=4

${tools_dir}/metar_get -n ls $metar_file >/dev/null
${tools_dir}/metar_get -w count=1/2/3 -p CCCC,latitude,longitude,dateTime,elevation,temperature,dewPointTemperature,qnh $metar_file

rm -f $fLog

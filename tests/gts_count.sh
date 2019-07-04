#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

#Enter data dir
cd ${data_dir}/gts

#Define a common label for all the tmp files
label="gts_count_test"

#Create log file
fLog=${label}".log"
rm -f $fLog

#Define tmp file
fTmp=${label}".tmp.txt"

gts_file=EGRR20150317121020_00493212.DAT
count=`${tools_dir}/gts_count $gts_file`
[ $count -eq 300 ]

rm -f $fLog

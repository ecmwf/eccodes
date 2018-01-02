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
cd ${data_dir}/gts

#Define a common label for all the tmp files
label="gts_ls_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp file
fTmp=${label}".tmp.txt"
rm -f $fTmp

#----------------------------------------------
# Test default "ls" on all the gts data files
#----------------------------------------------
gts_file=EGRR20150317121020_00493212.DAT
f=$gts_file

echo $f >> $fLog
${tools_dir}/gts_ls $f >> $fLog

#-------------------------------------------
# Test "-p" switch
#-------------------------------------------
ref_ls=$f".ls.ref"
res_ls=$f".ls.test"
REDIRECT=/dev/null

${tools_dir}/gts_ls -p TT,AA,II,CCCC,YY,GG,gg,BBB $f 2> $REDIRECT > $res_ls

diff $ref_ls $res_ls >$REDIRECT 2> $REDIRECT

rm -f $fLog $res_ls 

#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

#set -x


#Enter data dir
cd ${data_dir}/bufr

fLog="bufr_ls.log"
rm -f $fLog

fTmp="tmp.bufr_ls.txt"
rm -f $fTmp

#Create log filemore lev    
touch $fLog

#----------------------------------------------
# Test default "ls" on all the bufr data files
#----------------------------------------------
for f in `ls *.bufr` ; do
   echo $f >> $fLog
   ${tools_dir}/bufr_ls $f >> $fLog
done

#-------------------------------------------
# Test "-p" switch
#-------------------------------------------
f="aaen_55.bufr"
ref_ls=$f".ls.ref"
res_ls=$f".ls.test"
REDIRECT=/dev/null

${tools_dir}/bufr_ls -p totalLength,centre,subCentre,masterTableNumber,masterTablesVersionNumber,localTablesVersionNumber,numberOfSubsets,numberOfObservations $f 2> $REDIRECT > $fTmp

#Write the values into a file and compare with ref
awk NR==3 $fTmp | awk '{split($0,a," "); for (i=1; i<=8; i++) print a[i]}' > $res_ls
diff $ref_ls $res_ls >$REDIRECT 2> $REDIRECT

# counting messages
count=`${tools_dir}bufr_count syno_multi.bufr`
[ "$count" = "3" ]
count=`${tools_dir}codes_count syno_multi.bufr`
[ "$count" = "3" ]

rm -f $fLog $res_ls $fTmp

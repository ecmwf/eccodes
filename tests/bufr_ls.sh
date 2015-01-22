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

set -x

#Remember work dir
dWork=`pwd`

#Enter data dir
cd ${data_dir}/bufr

fLog="log"
rm -f $fLog | true

fTmp="tmp.txt"
rm -f $fTmp | true

#Create log filemore lev    
touch $fLog

#----------------------------------------------
# Test default "ls" on all the bufr data files
#----------------------------------------------

for f in `ls *.bufr` ; do
   echo $f >> log
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

#Write the values into a file and compare to ref
awk NR==3 $fTmp | awk '{split($0,a," "); for (i=1; i<=8; i++) print a[i]}' > $res_ls
diff $ref_ls $res_ls >$REDIRECT 2> $REDIRECT
[ $? -eq 0 ] 


#Go back to workdir
cd $dWork

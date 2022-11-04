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

# Enter data dir
cd ${data_dir}/bufr

# Define a common label for all the tmp files
label="bufr_ls_test"

fLog=${label}".log"
rm -f $fLog
touch $fLog
fTmp=${label}".tmp.txt"
rm -f $fTmp

#----------------------------------------------
# Test default "ls" on all the bufr data files
#----------------------------------------------
bufr_files=`cat bufr_data_files.txt`
for f in ${bufr_files} ; do
   echo $f >> $fLog
   ${tools_dir}/bufr_ls $f >> $fLog
done

#-----------------
# Test "-p" switch
#-----------------
f="aaen_55.bufr"
ref_ls=$f".ls.ref"
res_ls=$f".ls.test"
REDIRECT=/dev/null

${tools_dir}/bufr_ls -p totalLength,bufrHeaderCentre,bufrHeaderSubCentre,masterTableNumber,masterTablesVersionNumber,localTablesVersionNumber,numberOfSubsets,localNumberOfObservations $f 2> $REDIRECT > $fTmp

# Write the values into a file and compare with ref
awk NR==3 $fTmp | awk '{split($0,a," "); for (i=1; i<=8; i++) print a[i]}' > $res_ls
diff $ref_ls $res_ls

# ------------------------
# Test printing array key
# ------------------------
set +e
${tools_dir}/bufr_ls -p numericValues $f >$fTmp 2>&1
status=$?
set -e
[ $status -ne 0 ]
cat $fTmp
grep -q "Passed array is too small" $fTmp


# ------------------------
# Test setting header key
# ------------------------
${tools_dir}/bufr_ls -s satelliteID=313 -p satelliteID aaen_55.bufr > $fTmp
grep -q "313" $fTmp


# Clean up
rm -f $fLog $res_ls 
rm -f $fTmp

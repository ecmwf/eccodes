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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-875
# bufr_copy with -g option to copy the GTS header
# ---------------------------------------------------------
label="bufr_ecc-875-test"
tempOut=temp.${label}.out
tempRef=temp.${label}.ref
tempBufr=temp.${label}.bufr

# Case 1:
# --------------------------------
input=${data_dir}/gts/EGRR20150317121020_00493212.DAT

numBufrs=`${tools_dir}/bufr_count $input`
[ $numBufrs -eq 94 ]

${tools_dir}/bufr_copy -g -w numberOfSubsets=9 $input $tempBufr
# Remove the first line and the last three
${tools_dir}/gts_ls $tempBufr | sed 1d | sed 9,11d > $tempOut

cat > $tempRef <<EOF
TT          AA          II          CCCC        YY          GG          gg          BBB         identifier  
IU          SZ          41          KWBC        17          12          10          NNN         GTS        
IU          SZ          42          KWBC        17          12          10          NNN         GTS        
IU          SV          41          KWBC        17          12          10          NNN         GTS        
IU          SV          42          KWBC        17          12          10          NNN         GTS        
IU          KN          80          KWBC        17          12          09          NNN         GTS        
IU          KN          50          KWBC        17          12          00          RRA         GTS        
IU          KN          50          KWBC        17          12          00          RRB         GTS        
EOF

diff $tempRef $tempOut

# Clean up
rm -f $tempOut $tempRef $tempBufr

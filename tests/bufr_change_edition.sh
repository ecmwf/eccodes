#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

cd ${data_dir}/bufr

#Define a common label for all the tmp files
label="bufr_change_edition"

fBufrTmp=${label}".bufr.tmp"

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`

for f in ${bufr_files}; do
  if [ "$f" = "ias1_240.bufr" ]; then continue; fi
  
  # Convert to BUFR edition 4
  ${tools_dir}bufr_set -s editionNumber=4 $f $fBufrTmp
  ${tools_dir}bufr_compare -b edition $f $fBufrTmp
  
  ed=`${tools_dir}bufr_get -w count=1 -p editionNumber $fBufrTmp`
  [ "$ed" = "4" ]

done

rm -f $fBufrTmp

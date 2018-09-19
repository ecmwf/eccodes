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

label="bufr_subTypes"
fBufr3Input=$ECCODES_SAMPLES_PATH/BUFR3_local.tmpl
fBufr4Input=$ECCODES_SAMPLES_PATH/BUFR4_local.tmpl
fBufrOutput=${label}".bufr.tmp"

inputs="$fBufr3Input $fBufr4Input"
for fin in $inputs; do
  ${tools_dir}/bufr_set -s oldSubtype=2,newSubtype=300 $fin $fBufrOutput
  res=`${tools_dir}/bufr_get -p rdbSubtype,oldSubtype,newSubtype $fBufrOutput`
  [ "$res" = "2 2 300" ]
  
  ${tools_dir}/bufr_set -s oldSubtype=255,newSubtype=300 $fin $fBufrOutput
  res=`${tools_dir}/bufr_get -p rdbSubtype,oldSubtype,newSubtype $fBufrOutput`
  [ "$res" = "300 255 300" ]
done


rm -f $fBufrOutput

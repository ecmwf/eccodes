#!/bin/sh
# Copyright 2005-2017 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

#Define a common label for all the tmp files
label="bufr_wmo_tables_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp bufr files
fTmp=${label}".tmp"

#==============================================
# Testing latest WMO tables 
#==============================================
bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
REDIRECT=/dev/null
latest_wmo_version=25
blacklist=masterTablesVersionNumber,bandwidthCorrectionCoefficient1ForAtovs,bandwidthCorrectionCoefficient2ForAtovs,tovsOrAtovsProductQualifier,centre,subCentre
for file in ${bufr_files}
do
  ${tools_dir}/bufr_set -s  masterTablesVersionNumber=$latest_wmo_version ${data_dir}/bufr/$file $fTmp
  #${tools_dir}/bufr_dump $fTmp >/dev/null
  #${tools_dir}/bufr_compare -b $blacklist ${data_dir}/bufr/$file $fTmp
done


rm -f $fTmp

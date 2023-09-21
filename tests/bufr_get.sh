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
label="bufr_get_test"

# Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

# Define tmp file
fTmp=${label}".tmp.txt"
tempRef=${label}".tmp.ref.txt"
rm -f $fTmp $tempRef

# Define another tmp file to store the test results
res_get=${label}".get.test"
rm -f $res_get

# Check identidier. Edition 3 and 4
id=`${tools_dir}/bufr_get -p edition,identifier avhn_87.bufr`
[ "$id" = "3 BUFR" ]
id=`${tools_dir}/bufr_get -p edition,identifier aaen_55.bufr`
[ "$id" = "4 BUFR" ]

set +e
${tools_dir}/bufr_get -p identifier:d avhn_87.bufr
status=$?
set -e
[ $status -ne 0 ]

#-------------------------------------------
# Test "-p" switch
#-------------------------------------------
f="aaen_55.bufr"

# The reference is the same as for ls
ref_get=$f".ls.ref"

echo "Test: -p switch" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/bufr_get -p totalLength,bufrHeaderCentre,bufrHeaderSubCentre,masterTableNumber,masterTablesVersionNumber,localTablesVersionNumber,numberOfSubsets,localNumberOfObservations $f > $fTmp

# Write the values into a file and compare with ref
cat $fTmp | awk '{split($0,a," "); for (i=1; i<=8; i++) print a[i]}' > $res_get
diff $ref_get $res_get

#-------------------------------------------
# ECC-236
#-------------------------------------------
result=`${tools_dir}/bufr_get -s unpack=1 -p nonCoordinatePressure syno_1.bufr`
[ "$result" = "100910" ]
result=`${tools_dir}/bufr_get -s unpack=1 -p stationOrSiteName bssh_176.bufr`
[ "$result" = "Helgoland" ]
result=`${tools_dir}/bufr_get -s unpack=1 -p majorFrameCount aaen_55.bufr`
[ "$result" = "MISSING" ]
result=`${tools_dir}/bufr_get -s unpack=1 -p satelliteIdentifier wavb_134.bufr`
[ "$result" = "MISSING" ]

#-------------------------------------------
# ECC-315: BUFR keys in the MARS namespace
#-------------------------------------------
result=`${tools_dir}/bufr_get -m aaen_55.bufr`
[ "$result" = "55 20121102 0000" ]
result=`${tools_dir}/bufr_get -m syno_1.bufr`
[ "$result" = "1 20121030 0000 91334" ]
${tools_dir}/bufr_ls -j -m syno_1.bufr > $fTmp
cat > $tempRef <<EOF
{ "messages" : [ 
  {
    "obstype": 1,
    "date": 20121030,
    "time": "0000",
    "ident": 91334
  }
]}
EOF
cat $tempRef
diff $tempRef $fTmp

#-------------------------------------------
# Local ECMWF section: 'ident' key
#-------------------------------------------
result=`${tools_dir}/bufr_get -p isSatellite,ident syno_1.bufr`
[ "$result" = "0 91334" ]
result=`${tools_dir}/bufr_get -p isSatellite,ident temp_102.bufr`
[ "$result" = "0 ASDE3" ]
result=`${tools_dir}/bufr_get -p isSatellite,ident b004_145.bufr`
[ "$result" = "0 FAVRTLZA" ]

result=`${tools_dir}/bufr_get -f -p isSatellite,ident b003_56.bufr`
[ "$result" = "1 not_found" ]


# Check key 'file'
result=`${tools_dir}/bufr_get -p file b004_145.bufr`
[ "$result" = "b004_145.bufr" ]

# Decoding the "unpack" key also decodes the whole Data Section
result=`${tools_dir}/bufr_get -f -p heightOfStation aaen_55.bufr`
[ "$result" = "not_found" ]
result=`${tools_dir}/bufr_get -p unpack,heightOfStation aaen_55.bufr`
[ "$result" = "0 858000" ]
result=`${tools_dir}/bufr_get -p unpack:d,heightOfStation aaen_55.bufr`
[ "$result" = "0 858000" ]
result=`${tools_dir}/bufr_get -p unpack:s,heightOfStation aaen_55.bufr`
[ "$result" = "0 858000" ]


# Clean up
rm -f $fLog $fTmp $res_get $tempRef


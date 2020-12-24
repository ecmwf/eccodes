#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1187
# BUFR: codes_set_missing()
# ---------------------------------------------------------
label="bufr_ecc-1187-test"
tempOut=temp.${label}.out
tempRules=temp.${label}.filter

# Try each key type:
#  radiosondeType => LONG
#  radiosondeSerialNumber => STRING
#  #1#pressure => DOUBLE
input=${data_dir}/bufr/PraticaTemp.bufr

cat > $tempRules <<EOF
 set unpack=1;
 set radiosondeType=MISSING;
 set radiosondeSerialNumber=MISSING;
 set #1#pressure=MISSING;
 set pack=1;
 transient m1 = missing(radiosondeType);
 transient m2 = missing(radiosondeSerialNumber);
 transient m3 = missing(#1#pressure);
 assert ( m1 == 1 );
 assert ( m2 == 1 );
 assert ( m3 == 1 );
 write;
EOF

${tools_dir}/codes_bufr_filter -o $tempOut $tempRules $input
# ${tools_dir}/bufr_dump -p $tempOut

# Clean up
rm -f $tempRules $tempOut

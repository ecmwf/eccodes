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
set -u

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-556.
# It tests typicalDate for BUFR edition 4
# ---------------------------------------------------------
label="bufr_ecc-556-test"

# In this BUFR message the typicalYear should have been encoded
# as 2012 but it is actually 12!
input=${data_dir}/bufr/aaen_55.bufr
res=`${tools_dir}/bufr_get -p edition,typicalYear,typicalDate $input`
[ "$res" = "4 12 20121102" ]

# Now try a BUFR4 file with correctly coded year
input=${data_dir}/bufr/PraticaTemp.bufr
res=`${tools_dir}/bufr_get -p edition,typicalYear,typicalDate $input`
[ "$res" = "4 2015 20151202" ]

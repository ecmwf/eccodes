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

label="bufr_indexing_test"
tempIndex=temp.$label.$$.idx
tempBufr=temp.$label.$$.bufr
tempOut=temp.$label.$$.out
tempRef=temp.$label.$$.ref

# ------------------
# Data Section key
# ------------------
infile=${data_dir}/bufr/tropical_cyclone.bufr
${tools_dir}/bufr_index_build -N -o $tempIndex -k stormIdentifier ${infile}

# Must remove first line (filename specifics)
${tools_dir}/bufr_dump ${tempIndex} | sed '1d' > $tempOut

cat > $tempRef <<EOF
Index keys:
key name = stormIdentifier
values = 27W, 70E, 71W
Index count = 3
EOF

diff $tempRef $tempOut

# ------------------
# Header MARS key
# ------------------
infile=${data_dir}/bufr/syno_multi.bufr
${tools_dir}/bufr_index_build -k mars.ident -o $tempIndex $infile |\
   grep -q "mars.ident = { 01001, 01003, 01007 }"


# ------------------
# Error conditions
# ------------------
infile=${data_dir}/bufr/vos308014_v3_26.bufr
set +e
${tools_dir}/bufr_index_build -o $tempIndex $infile > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Unable to unpack BUFR to create index" $tempOut


echo BUFR > $tempBufr
set +e
${tools_dir}/bufr_index_build $tempBufr > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "End of resource reached" $tempOut


# Clean up
rm -f $tempIndex $tempOut $tempRef $tempBufr

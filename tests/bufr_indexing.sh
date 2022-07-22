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


# Clean up
rm -f $tempIndex $tempOut $tempRef

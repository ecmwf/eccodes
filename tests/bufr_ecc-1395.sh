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

label="bufr_ecc-1395_test"

tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempOut=temp.$label.out

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

rm -f $tempBufr

# Pick a descriptor that does not exist in version 19
cat > $tempFilt <<EOF
 set masterTablesVersionNumber = 19;
 set unexpandedDescriptors = { 25195 }; # should fail
 write;
EOF

set +e
${tools_dir}/bufr_filter -o $tempBufr $tempFilt $sample_bufr4 > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]

[ ! -f "$tempBufr" ]

grep -q "unable to get descriptor 025195" $tempOut


rm -f $tempFilt $tempOut

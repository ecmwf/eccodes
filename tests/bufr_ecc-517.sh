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

label="bufr_ecc-517_test"
tempBufr=temp.$label.bufr
tempText=temp.$label.txt

# Run program to create output BUFR file with replication
export ECCODES_DEBUG=-1
$EXEC ${test_dir}/bufr_ecc-517 $tempBufr > $tempText 2>&1
unset ECCODES_DEBUG
rm -f $tempText

# Check file is OK and has the expected number of descriptors
${tools_dir}/bufr_dump -jf $tempBufr > $tempText
count=$(grep -c extendedVerticalSoundingSignificance $tempText)
[ $count -eq 487 ]

# Clean up
rm $tempBufr $tempText

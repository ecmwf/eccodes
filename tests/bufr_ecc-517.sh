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

OUTPUT=temp.ecc-517.bufr
TEMP=temp.ecc-517.txt

# Run program to create output BUFR file with replication
${test_dir}/bufr_ecc-517 $OUTPUT

# Check file is OK and has the expected number of descriptors
${tools_dir}/bufr_dump -jf $OUTPUT > $TEMP
count=`grep -c extendedVerticalSoundingSignificance $TEMP`
[ $count -eq 487 ] 


rm $OUTPUT $TEMP

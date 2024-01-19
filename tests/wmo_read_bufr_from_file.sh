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

label="wmo_read_bufr_from_file_test"
tempText=temp.$label.txt
tempBufr=temp.$label.bufr

${test_dir}/wmo_read_bufr_from_file $data_dir/bufr/ias1_240.bufr > $tempText
grep -q "BUFR: size: 180696 .*No error" $tempText

echo BUFR > $tempBufr
${test_dir}/wmo_read_bufr_from_file $tempBufr > $tempText
grep -q "BUFR: size: 0 .*End of resource reached when reading message" $tempText

# Clean up
rm -f $tempText $tempBufr

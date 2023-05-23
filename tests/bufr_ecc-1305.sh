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

label="bufr_ecc-1305_test"
temp=temp.$label

# Check the operator name is printed
infile=${data_dir}/bufr/maer_207.bufr
${tools_dir}/bufr_dump -d $infile > $temp

grep -q "224000.*firstOrderStatisticalValuesFollow" $temp
grep -q "224255.*firstOrderStatisticalValuesMarkerOperator" $temp

rm -f $temp

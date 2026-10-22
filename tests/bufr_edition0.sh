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

label="bufr_edition0_test"
temp=temp.$label.txt

$EXEC ${test_dir}/bufr_edition0 > $temp
grep -q "cloudType=60" $temp
grep -q "horizontalVisibility=30000" $temp
grep -q "ident=.*08495" $temp

# Clean up test outputs
rm -f $temp

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

label="gts_dump_test"

fLog=${label}.log
fTmp=${label}.tmp.txt

# Enter data dir
cd ${data_dir}/gts

gts_file=EGRR20150317121020_00493212.DAT
${tools_dir}/gts_dump -w count=1 $gts_file
${tools_dir}/gts_dump -w count=2 $gts_file
${tools_dir}/gts_dump -Dat $gts_file
${tools_dir}/gts_dump -OH  $gts_file
${tools_dir}/gts_dump -d   $gts_file

set +e
${tools_dir}/gts_dump -OD $gts_file
status=$?
set -e
[ $status -ne 0 ]

# Clean up
rm -f $fLog $fTmp

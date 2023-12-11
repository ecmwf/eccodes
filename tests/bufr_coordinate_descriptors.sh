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

label="bufr_coordinate_descriptors_test"
temp=temp.$label.txt

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
for f in ${bufr_files}; do
    fpath=${data_dir}/bufr/$f
    ${test_dir}/bufr_coordinate_descriptors $fpath
done
rm -f $temp

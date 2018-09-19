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
set -u

#Define a common label for all the tmp files
label="bufr_split_by_subtype"
temp=$label.temp
fRules=${label}.filter

# Do all the work in a temporary directory
temp_dir=tempdir.${label}
mkdir -p $temp_dir
cd $temp_dir

cat > $fRules <<EOF
  write "out.filter_by_subtype.[rdbSubtype].bufr";
EOF

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
for f in ${bufr_files}; do
    fpath=${data_dir}/bufr/$f

    # This will create output files like out.filter_by_subtype.*
    ${tools_dir}/codes_bufr_filter $fRules $fpath

    # This will create output files like split_rdbSubtype*.bufr
    ${tools_dir}/bufr_split_by_subtype -v $fpath

    for sp in out.filter_by_subtype.*; do
        st=`echo $sp | awk -F. '{print $3}'`
        ${tools_dir}/bufr_compare $sp split_rdbSubtype.$st.bufr
    done

    rm -f split_rdbSubtype*.bufr
    rm -f out.filter_by_subtype.*
done

# Clean up
# -------------
cd $test_dir
rm -fr $temp_dir

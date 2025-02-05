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

# -------------------------------------------------------------------
# This is the test for the JIRA issue ECC-2006.
# Indexing: Performance degradation after reading thousands of files
# -------------------------------------------------------------------

label="grib_ecc-2006_test"

tempIndex=temp.$label.idx
tempLog=temp.$label.log
tempOut=temp.$label.txt

export ECCODES_DEBUG=-1

gfiles="
    $data_dir/tigge_af_ecmwf.grib2
    $data_dir/tigge_cf_ecmwf.grib2
    $data_dir/tigge_ecmwf.grib2
    $data_dir/tigge_pf_ecmwf.grib2"

${tools_dir}/grib_index_build -o $tempIndex $gfiles >$tempLog 2>&1

# There should be no files in the pool after we call grib_index_delete
grep "file_pool before grib_index_delete: size=4, num_opened_files=4" $tempLog
grep "file_pool after grib_index_delete: size=0, num_opened_files=0" $tempLog

# Clean up
rm -f $tempIndex $tempLog $tempOut

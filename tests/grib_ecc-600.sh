#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-600.
# Nearest neighbour for rotated lat/lon grids.
# ---------------------------------------------------------
label="grib_ecc-600-test"
tempOut=temp.${label}.out
tempGrib=temp.${label}.grib

input=${data_dir}/tigge/tiggelam_cnmc_sfc.grib
${tools_dir}/grib_copy -w count=4 $input $tempGrib

${tools_dir}/grib_ls -l 40,0,1   $tempGrib > $tempOut
grep -q "Grid Point chosen #3 index=54294 latitude=39.98 longitude=0.00 distance=2.03 (Km)" $tempOut

${tools_dir}/grib_ls -l 50,-10,1 $tempGrib > $tempOut
grep -q "Grid Point chosen #3 index=145684 latitude=49.99 longitude=-9.97 distance=2.57 (Km)" $tempOut


# Clean up
rm -f $tempOut $tempGrib

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
# This is the test for the JIRA issue ECC-1000
# Nearest neighbour using grib_get/grib_ls on severa files
# ---------------------------------------------------------
label="grib_ecc-1000-test"
tempOut=temp.${label}.out
tempRef=temp.${label}.ref

input_ll=${data_dir}/regular_latlon_surface.grib1
input_gg=${data_dir}/reduced_gaussian_model_level.grib1

# Test 'grib_get' tool
# --------------------
${tools_dir}/grib_get -F%.1f -l0,0 $input_ll $input_gg > $tempOut
cat > $tempRef <<EOF
300.8 300.1 300.9 300.9 
198.3 198.5 199.3 199.6 
EOF
diff -w $tempRef $tempOut
rm -f $tempOut $tempRef

${tools_dir}/grib_get -F%.1f -l0,0 $input_gg $input_ll > $tempOut
cat > $tempRef <<EOF
198.3 198.5 199.3 199.6 
300.8 300.1 300.9 300.9 
EOF
diff -w $tempRef $tempOut


# Test grib_ls
# ---------------------
${tools_dir}/grib_ls -F%.1f -l0,0 $input_ll $input_gg



# Clean up
rm -f $tempOut $tempRef

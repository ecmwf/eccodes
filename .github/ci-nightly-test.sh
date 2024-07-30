#!/bin/bash

set -eu

# We do not want to come across the ecCodes tools in the toolbox
module unload ecmwf-toolbox

module load cdo/new
module load numdiff
module load nccmp
module load netcdf4
module load gnuparallel/new
module load python3

version=$(cat $TMPDIR/eccodes/VERSION)

# Note:
# The environment variable "GH_TOKEN" needs to be there for the clone to succeed
#

#regression_suite_dir=$TMPDIR/eccodes-regression-tests
#mkdir -p $regression_suite_dir
#git clone https://${GH_TOKEN}@github.com/ecmwf/eccodes-regression-tests.git $regression_suite_dir
#cd $regression_suite_dir

# Launch the regression tests in parallel (This script uses GNU parallel)
cd ~masn/REGRESSION_TESTING/ecCodes
./par-suite.sh -w $TMPDIR/install/eccodes/$version

# For debugging specific test(s)
# ./seq-suite.sh -w $TMPDIR/install/eccodes/$version -d -t $test_name

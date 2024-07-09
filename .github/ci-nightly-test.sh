#!/bin/bash

set -e

# We do not want to come across the ecCodes tools in the toolbox
module unload ecmwf-toolbox

module load cdo/new
module load numdiff
module load nccmp
module load netcdf4
module load gnuparallel/new
module load python3

version=$(cat $TMPDIR/eccodes/VERSION)

regression_suite_dir=$TMPDIR/eccodes-regression-tests
mkdir -p $regression_suite_dir
git clone git@github.com:ecmwf/eccodes-regression-tests.git $regression_suite_dir
./par-suite.sh -w $TMPDIR/install/eccodes/$version

# For debugging specific test(s)
# ./seq-suite.sh -w $TMPDIR/install/eccodes/$version -d -t py_

#!/bin/bash

set -eu

usage() {
    echo "Usage: $0 <testing|performance>"
    exit 1
}

if [[ $# -ne 1 ]]; then
    usage
fi

mode="$1"
if [[ "$mode" != "testing" && "$mode" != "performance" ]]; then
    echo "Error: unknown mode '$mode'"
    usage
fi

# We do not want to come across the ecCodes tools in the toolbox
module unload ecmwf-toolbox

module load cdo/new
module load numdiff
module load nccmp
module load netcdf4
module load python3

if [[ "$mode" == "testing" ]]; then
    module load gnuparallel/new
fi

version=$(cat $TMPDIR/eccodes/VERSION)

# Note:
# The environment variable "GH_TOKEN" needs to be there for the clone to succeed
#

regression_suite_dir=$TMPDIR/eccodes-regression-tests
mkdir -p $regression_suite_dir
git clone https://${GH_TOKEN}@github.com/ecmwf/eccodes-regression-tests.git $regression_suite_dir
cd $regression_suite_dir

if [[ "$mode" == "testing" ]]; then
    # Launch the regression tests in parallel (This script uses GNU parallel)
    echo "Running ./par-suite.sh -w $TMPDIR/install/eccodes/$version"
    ./par-suite.sh -w $TMPDIR/install/eccodes/$version
elif [[ "$mode" == "performance" ]]; then
    # We should ideally run performance tests MULTIPLE times
    echo "Running ./seq-suite.sh -w $TMPDIR/install/eccodes/$version -t PERFORMANCE"
    ./seq-suite.sh -w $TMPDIR/install/eccodes/$version -t PERFORMANCE
fi

# For debugging specific test(s)
# ./seq-suite.sh -w $TMPDIR/install/eccodes/$version -d -t $test_name

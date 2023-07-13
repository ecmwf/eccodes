#!/bin/bash

set -e

# We do not want to come across the ecCodes tools in the toolbox
module unload ecmwf-toolbox

module load cdo/new
module load numdiff
module load nccmp
module load netcdf4/new
module load gnuparallel/new
module load python3

cd ~masn/REGRESSION_TESTING/ecCodes
./par-suite.sh -w $TMPDIR/install/eccodes

# For debugging specific test(s)
# ./seq-suite.sh -w $TMPDIR/install/eccodes -d -t py_

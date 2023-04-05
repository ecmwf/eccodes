#!/bin/bash

set -e

module unload ecmwf-toolbox # Very important if you are testing your own build/install
module load  cdo/new
module load  numdiff
module load  nccmp netcdf4/new
module load  gnuparallel/new
module load  python3

cd ~masn/REGRESSION_TESTING/ecCodes
# ./par-suite.sh -w $TMPDIR/install/eccodes
./seq-suite.sh -w $TMPDIR/install/eccodes -d -t ecc-1314-grib.sh
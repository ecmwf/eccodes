#!/bin/bash

# initialise module environment if it is not
if [[ ! $(command -v module > /dev/null 2>&1) ]]; then
  . /usr/local/apps/module/init/bash
fi

module unload grib_api
module unload eccodes
module unload emos
module unload fftw
module unload libemos
module unload metview
module unload netcdf4

module list

module load cmake/3.16.5

module switch gnu intel/17.0.3

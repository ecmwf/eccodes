#!/bin/bash

[[ $(uname) == "Darwin" ]] && return

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

module load cmake/3.16.5

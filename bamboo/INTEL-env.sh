# Initialise module environment if it is not
if [[ ! $(command -v module > /dev/null 2>&1) ]]; then
  . /usr/local/apps/module/init/bash
fi
# Unload modules not available for Intel
module unload grib_api
module unload eccodes
module unload emos
module unload fftw
module unload libemos
module switch gnu intel/16.0.3

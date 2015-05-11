# Initialise module environment if it is not
if [[ ! $(command -v module > /dev/null 2>&1) ]]; then
  . /usr/local/apps/module/init/bash
fi
# eccodes module conflicts with intel module
module unload eccodes
module switch gnu intel/15.0.2

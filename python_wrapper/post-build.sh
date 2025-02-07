#!/bin/bash

# NOTE auditwheel is problematic since it changes libnames -- all is well from
# the pov # of this very package's libs, but subsequent packages compiled with
# this as a dependency end up not working
# if [ "$(uname)" != "Darwin" ] ; then
#   auditwheel repair -w /tmp/eccodes/auditwheel /tmp/eccodes/build/wheel/*whl
# fi

# NOTE on macos we delocate with impunity, because the findlibs recursive depload
# is disabled anyway
if [ "$(uname)" = "Darwin" ] ; then
    delocate-wheel /tmp/eccodes/build/wheel/*whl
fi

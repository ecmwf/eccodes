#!/bin/bash

# NOTE auditwheel/delocate are problematic since firstly they don't copy
# include/.h files so we'd need to handle them manually anyway, secondly they
# would include also eckit etc which we want to stay in an isolated wheel,
# and thirdly they mutate library names, posing a challenge for downstream
# compilation

# if [ "$(uname)" != "Darwin" ] ; then
#   auditwheel repair -w /tmp/eccodes/auditwheel /tmp/eccodes/build/wheel/*whl
# fi
# if [ "$(uname)" = "Darwin" ] ; then
#    delocate-wheel /tmp/eccodes/build/wheel/*whl
# fi

#!/bin/bash

# NOTE for macos, we additionally fix the install names. For linux we don't need to, since the rpath is already pointing
# to the current directory and there is no strict install name check. The libs were copied already in the precompile
# step

set -euo pipefail

if [ "$(uname)" = "Darwin" ] ; then
    BASE="/tmp/eccodes/target/eccodes/lib"
    for e in $(ls $BASE/*dylib) ; do
        echo "Fixing deps of $e"
        for p in libopenjp libaec libpng ; do
            # check that the lib $e has $p as a dependency, but also that $e is *not* $p itself
            if (otool -l $e | grep "name.*$p.*offset" >/dev/null 2>/dev/null) && (echo $e | grep -v $p >/dev/null 2>/dev/null) ; then
                ORIG_NAME=$(otool -l $e | grep "name.*$p.*" | sed 's/.*name \(.*\) (offset.*)/\1/')
                ORIG_BASE=$(basename $ORIG_NAME)
                echo "Fixing $p as a dependency of $e: $ORIG_NAME => @rpath/$ORIG_BASE"
                install_name_tool -change $ORIG_NAME '@rpath/'$ORIG_BASE $e
            else
                echo "$e does not have $p as dependency"
            fi

        done
    done
fi
